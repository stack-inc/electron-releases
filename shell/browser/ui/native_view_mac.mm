#include "shell/browser/ui/native_view.h"

#include "base/mac/foundation_util.h"
#include "base/mac/scoped_cftyperef.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/cocoa/events_handler.h"
#include "shell/browser/ui/view_utils.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rect_f.h"

namespace electron {

namespace {

// There is no way to know when another application has installed an event
// monitor, we have to assume only current app can capture view.
NativeView* g_captured_view = nullptr;

EventType EventTypeFromNS(NSEvent* event) {
  switch ([event type]) {
    case NSLeftMouseDown:
      return EventType::kLeftMouseDown;
    case NSRightMouseDown:
      return EventType::kRightMouseDown;
    case NSOtherMouseDown:
      return EventType::kOtherMouseDown;
    case NSLeftMouseUp:
      return EventType::kLeftMouseUp;
    case NSRightMouseUp:
      return EventType::kRightMouseUp;
    case NSOtherMouseUp:
      return EventType::kOtherMouseUp;
    case NSLeftMouseDragged:
    case NSRightMouseDragged:
    case NSOtherMouseDragged:
    case NSMouseMoved:
      return EventType::kMouseMove;
    case NSMouseEntered:
      return EventType::kMouseEnter;
    case NSMouseExited:
      return EventType::kMouseLeave;
    default:
      return EventType::kUnknown;
  }
}

gfx::Point GetPosInView(NSEvent* event, NSView* view) {
  NSPoint point = [view convertPoint:[event locationInWindow] fromView:nil];
  if ([view isFlipped])
    return gfx::Point(point.x, point.y);
  NSRect frame = [view frame];
  return gfx::Point(point.x, NSHeight(frame) - point.y);
}

gfx::Point GetPosInWindow(NSEvent* event, NSView* view) {
  NSPoint point = [event locationInWindow];
  if ([view isFlipped])
    return gfx::Point(point.x, point.y);
  NSWindow* window = [event window];
  NSRect frame = [window contentRectForFrameRect:[window frame]];
  return gfx::Point(point.x, NSHeight(frame) - point.y);
}

}  // namespace

NativeEvent::NativeEvent(NATIVEEVENT event, NATIVEVIEW view)
    : type(EventTypeFromNS(event)),
      timestamp([event timestamp] * 1000),
      native_event(event) {}

NativeMouseEvent::NativeMouseEvent(NATIVEEVENT event, NATIVEVIEW view)
    : NativeEvent(event, view),
      button([event buttonNumber] + 1),
      position_in_view(GetPosInView(event, view)),
      position_in_window(GetPosInWindow(event, view)) {}

NativeView::RoundedCornersOptions::RoundedCornersOptions() = default;

NativeView::ClippingInsetOptions::ClippingInsetOptions() = default;

void NativeView::SetNativeView(NATIVEVIEW view) {
  view_ = view;

  if (!IsNativeView(view))
    return;

  // Install events handle for the view's class.
  InstallNativeViewMethods([view class]);

  // Initialize private bits of the view.
  NativeViewPrivate* priv = [view nativeViewPrivate];
  priv->shell = this;

  // Set the |focusable| property to the parent class's default one.
  SEL cmd = @selector(acceptsFirstResponder);
  auto super_impl = reinterpret_cast<BOOL (*)(NSView*, SEL)>(
      [[view superclass] instanceMethodForSelector:cmd]);
  priv->focusable = super_impl(view, cmd);
}

void NativeView::InitView() {
  if (!IsVibrant())
    SetNativeView([[ElectronNativeView alloc] init]);
  else
    SetNativeView([[ElectronNativeVibrantView alloc] init]);
}

void NativeView::DestroyView() {
  if (IsNativeView(view_)) {
    // Release all hooks before destroying the view.
    [view_ disableTracking];
    ReleaseCapture();
    // The view may be referenced after this class gets destroyed.
    NativeViewPrivate* priv = [view_ nativeViewPrivate];
    priv->shell = nullptr;
  }
  [view_ release];
}

void NativeView::SetBounds(const gfx::Rect& bounds,
                           const gin_helper::Dictionary& options) {
  SetBoundsForView(view_, bounds, options);
  NSRect frame = bounds.ToCGRect();
  // Calling setFrame manually does not trigger resizeSubviewsWithOldSize.
  [view_ resizeSubviewsWithOldSize:frame.size];
}

gfx::Rect NativeView::GetBounds() const {
  auto* superview = view_.superview;
  if (superview && ![superview isFlipped]) {
    const int superview_height = superview.frame.size.height;
    return gfx::Rect(
        view_.frame.origin.x,
        superview_height - view_.frame.origin.y - view_.frame.size.height,
        view_.frame.size.width, view_.frame.size.height);
  }
  return ToNearestRect(gfx::RectF(view_.frame));
}

gfx::Point NativeView::OffsetFromView(const NativeView* from) const {
  NSPoint point = [view_ convertPoint:NSZeroPoint toView:from->view_];
  return gfx::Point(point.x, point.y);
}

gfx::Point NativeView::OffsetFromWindow() const {
  NSPoint point = [view_ convertPoint:NSZeroPoint toView:nil];
  return gfx::Point(point.x, point.y);
}

void NativeView::SetVisibleImpl(bool visible) {
  [view_ setHidden:!visible];
}

bool NativeView::IsVisible() const {
  return ![view_ isHidden];
}

bool NativeView::IsTreeVisible() const {
  return ![view_ isHiddenOrHasHiddenAncestor];
}

void NativeView::Focus() {
  if (view_.window && IsFocusable())
    [view_.window makeFirstResponder:view_];
}

bool NativeView::HasFocus() const {
  if (view_.window)
    return view_.window.firstResponder == view_;
  else
    return false;
}

void NativeView::SetFocusable(bool focusable) {
  NativeViewPrivate* priv = [view_ nativeViewPrivate];
  priv->focusable = focusable;
}

bool NativeView::IsFocusable() const {
  return [view_ acceptsFirstResponder];
}

void NativeView::SetBackgroundColor(SkColor color) {
  if (IsNativeView(view_))
    [view_ setNativeBackgroundColor:color];
}

void NativeView::SetVisualEffectMaterial(VisualEffectMaterial material) {
  if (IsNativeView(view_) && vibrant_) {
    static_cast<ElectronNativeVibrantView*>(view_).material =
        static_cast<NSVisualEffectMaterial>(material);
  }
}

VisualEffectMaterial NativeView::GetVisualEffectMaterial() const {
  if (IsNativeView(view_) && vibrant_) {
    return static_cast<VisualEffectMaterial>(
        static_cast<ElectronNativeVibrantView*>(view_).material);
  } else {
    return VisualEffectMaterial::kAppearanceBased;
  }
}

void NativeView::SetVisualEffectBlendingMode(VisualEffectBlendingMode mode) {
  if (IsNativeView(view_) && vibrant_) {
    SetWantsLayer(mode == VisualEffectBlendingMode::kWithinWindow);
    static_cast<ElectronNativeVibrantView*>(view_).blendingMode =
        static_cast<NSVisualEffectBlendingMode>(mode);
  }
}

VisualEffectBlendingMode NativeView::GetVisualEffectBlendingMode() const {
  if (IsNativeView(view_) && vibrant_) {
    return static_cast<VisualEffectBlendingMode>(
        static_cast<ElectronNativeVibrantView*>(view_).blendingMode);
  } else {
    return VisualEffectBlendingMode::kBehindWindow;
  }
}

void NativeView::SetCapture() {
  if (g_captured_view)
    g_captured_view->ReleaseCapture();

  NativeViewPrivate* priv = [view_ nativeViewPrivate];
  priv->mouse_capture.reset(new MouseCapture(this));
  g_captured_view = this;
}

void NativeView::ReleaseCapture() {
  if (g_captured_view != this)
    return;

  NativeViewPrivate* priv = [view_ nativeViewPrivate];
  priv->mouse_capture.reset();
  g_captured_view = nullptr;
  NotifyCaptureLost();
}

bool NativeView::HasCapture() const {
  return g_captured_view == this;
}

void NativeView::EnableMouseEvents() {
  AddMouseEventHandlerToClass([view_ class]);
}

void NativeView::SetMouseTrackingEnabled(bool enable) {
  if (enable) {
    // Install event tracking area.
    [view_ enableTracking];
    EnableMouseEvents();
  } else {
    [view_ disableTracking];
  }
}

bool NativeView::IsMouseTrackingEnabled() {
  NativeViewPrivate* priv = [view_ nativeViewPrivate];
  return !!(priv->tracking_area);
}

void NativeView::SetWantsLayer(bool wants) {
  [view_ nativeViewPrivate]->wants_layer = wants;
  [view_ setWantsLayer:wants];
}

bool NativeView::WantsLayer() const {
  return [view_ wantsLayer];
}

void NativeView::SetRoundedCorners(
    const NativeView::RoundedCornersOptions& options) {
  if (@available(macOS 10.13, *)) {
    auto* view = GetNative();
    view.wantsLayer = YES;
    view.layer.masksToBounds = YES;
    view.layer.cornerRadius = options.radius;

    CACornerMask mask = 0;
    if (options.top_left)
      mask |= kCALayerMinXMaxYCorner;
    if (options.top_right)
      mask |= kCALayerMaxXMaxYCorner;
    if (options.bottom_left)
      mask |= kCALayerMinXMinYCorner;
    if (options.bottom_right)
      mask |= kCALayerMaxXMinYCorner;
    view.layer.maskedCorners = mask;
  }
}

void NativeView::SetClippingInsets(
    const NativeView::ClippingInsetOptions& options) {
  if (@available(macOS 10.13, *)) {
    auto* view = GetNative();

    CALayer* sublayer = [CALayer layer];
    sublayer.backgroundColor =
        [[NSColor blackColor] colorWithAlphaComponent:1.0].CGColor;

    if (options.left == 0 && options.right == 0 && options.bottom == 0 &&
        options.top == 0) {
      view.layer.mask = nil;
      return;
    }

    int newFrameX = view.frame.origin.x + options.left;
    int newFrameY = view.frame.origin.y + options.bottom;
    int newFrameWidth = view.frame.size.width - options.left - options.right;
    int newFrameHeight = view.frame.size.height - options.bottom - options.top;
    sublayer.frame =
        NSMakeRect(newFrameX, newFrameY, newFrameWidth, newFrameHeight);

    view.layer.mask = sublayer;
  }
}

void NativeView::ResetScaling() {
  ResetScalingForView(view_);
}

void NativeView::SetScale(const gin_helper::Dictionary& options) {
  SetScaleForView(view_, options);
}

float NativeView::GetScaleX() {
  return GetScaleXForView(view_);
}

float NativeView::GetScaleY() {
  return GetScaleYForView(view_);
}

void NativeView::SetOpacity(const double opacity,
                            const gin_helper::Dictionary& options) {
  SetOpacityForView(view_, opacity, options);
}

double NativeView::GetOpacity() {
  return GetOpacityForView(view_);
}

void NativeView::UpdateDraggableRegions() {}

}  // namespace electron
