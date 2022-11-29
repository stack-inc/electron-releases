// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_base_view.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>
#include <objc/objc-runtime.h>

#include <algorithm>
#include <cstdint>

#include "base/cxx17_backports.h"
#include "base/logging.h"
#include "base/mac/foundation_util.h"
#include "base/mac/scoped_cftyperef.h"
#include "base/numerics/safe_conversions.h"
#include "gin/handle.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/cocoa/events_handler.h"
#include "shell/common/color_util.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rect_f.h"

namespace electron {

namespace api {

namespace {

const NSSize unitSize = {1.0, 1.0};

// There is no way to know when another application has installed an event
// monitor, we have to assume only current app can capture view.
BaseView* g_captured_view = nullptr;

}  // namespace

void BaseView::CreateView() {
  if (IsVibrant()) {
    SetView([[ElectronNativeVibrantView alloc] init]);
  } else if (IsBlurred()) {
    SetView([[ElectronNativeBlurredView alloc] init]);
  } else if (IsScaled()) {
LOG(ERROR) << "@@creating scaled view";
    ElectronScaleRotateFlipView* view = [[ElectronScaleRotateFlipView alloc] init];
    SetView(view);
    [view setFlipped:YES];
    [view setScaleValue:1.0];
LOG(ERROR) << "@@created scaled view";
  } else {
    SetView([[ElectronNativeView alloc] init]);
  }
}

std::uintptr_t BaseView::GetNativeID() const {
  return reinterpret_cast<std::uintptr_t>(nsview_);
}

void BaseView::SetClickThrough(bool click_through) {
  is_click_through_ = click_through;
}

bool BaseView::IsClickThrough() const {
  return is_click_through_;
}

void BaseView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  BoundsAnimationOptions options;
  args->GetNext(&options);

  NSRect frame = bounds.ToCGRect();
  auto* superview = nsview_.superview;
  if (superview && ![superview isFlipped]) {
    const auto superview_height = superview.frame.size.height;
    frame.origin.y = superview_height - bounds.y() - bounds.height();
  }

  if (!options.animation) {
    [nsview_ setFrame:frame];
    [nsview_ setNeedsDisplay:YES];
    // Calling setFrame manually does not trigger resizeSubviewsWithOldSize.
    [nsview_ resizeSubviewsWithOldSize:frame.size];
    return;
  }

  NSRect fromFrame = nsview_.frame;
  if (options.use_from_bounds) {
    if (superview && ![superview isFlipped]) {
      const auto superview_height = superview.frame.size.height;
      fromFrame =
          NSMakeRect(options.from_bounds.x(),
                     superview_height - options.from_bounds.y() -
                         options.from_bounds.height(),
                     options.from_bounds.width(), options.from_bounds.height());
    } else {
      fromFrame = options.from_bounds.ToCGRect();
    }
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
  }

  CABasicAnimation* positionAnimation =
      [CABasicAnimation animationWithKeyPath:@"position"];
  positionAnimation.duration = options.duration;
  positionAnimation.timingFunction = timing_function;
  positionAnimation.fromValue = @(fromFrame.origin);
  positionAnimation.toValue = @(frame.origin);
  if (fromFrame.size.width == frame.size.width &&
      fromFrame.size.height == frame.size.height) {
    [nsview_.layer addAnimation:positionAnimation forKey:@"position"];
  } else {
    CABasicAnimation* sizeAnimation =
        [CABasicAnimation animationWithKeyPath:@"bounds.size"];
    sizeAnimation.duration = options.duration;
    sizeAnimation.timingFunction = timing_function;
    sizeAnimation.fromValue = @(fromFrame.size);
    sizeAnimation.toValue = @(frame.size);
    CAAnimationGroup* group = [CAAnimationGroup animation];
    group.duration = options.duration;
    group.timingFunction = timing_function;
    group.animations = @[ positionAnimation, sizeAnimation ];
    [nsview_.layer addAnimation:group forKey:@"allMyAnimations"];
  }
  [nsview_ setFrame:frame];
}

gfx::Rect BaseView::GetBounds() const {
  auto* superview = nsview_.superview;
  if (superview && ![superview isFlipped]) {
    const int superview_height = superview.frame.size.height;
    return gfx::Rect(
        base::ClampRound(nsview_.frame.origin.x),
        base::ClampRound(superview_height - nsview_.frame.origin.y -
                         nsview_.frame.size.height),
        base::ClampRound(nsview_.frame.size.width),
        base::ClampRound(nsview_.frame.size.height));
  }
  return ToNearestRect(gfx::RectF(nsview_.frame));
}

void BaseView::SetViewBounds(const gfx::Rect& bounds) {
  NSRect vbounds = bounds.ToCGRect();
  auto* superview = nsview_.superview;
  if (superview && ![superview isFlipped]) {
    const auto superview_height = superview.bounds.size.height;
    vbounds.origin.y = superview_height - bounds.y() - bounds.height();
  }

  [nsview_ setBounds:vbounds];
  [nsview_ setNeedsDisplay:YES];
  [nsview_ resizeSubviewsWithOldSize:vbounds.size];
}

gfx::Rect BaseView::GetViewBounds() const {
  auto* superview = nsview_.superview;
  if (superview && ![superview isFlipped]) {
    const int superview_height = superview.bounds.size.height;
    return gfx::Rect(
        base::ClampRound(nsview_.bounds.origin.x),
        base::ClampRound(superview_height - nsview_.bounds.origin.y -
                         nsview_.bounds.size.height),
        base::ClampRound(nsview_.bounds.size.width),
        base::ClampRound(nsview_.bounds.size.height));
  }
  return ToNearestRect(gfx::RectF(nsview_.bounds));
}

gfx::Point BaseView::OffsetFromView(gin::Handle<BaseView> from) const {
  NSPoint point = [nsview_ convertPoint:NSZeroPoint toView:from->GetNSView()];
  return gfx::Point(point.x, point.y);
}

gfx::Point BaseView::OffsetFromWindow() const {
  NSPoint point = [nsview_ convertPoint:NSZeroPoint toView:nil];
  return gfx::Point(point.x, point.y);
}

void BaseView::SetVisible(bool visible) {
  if (visible == IsVisible())
    return;
  [nsview_ setHidden:!visible];
}

bool BaseView::IsVisible() const {
  return ![nsview_ isHidden];
}

bool BaseView::IsTreeVisible() const {
  return ![nsview_ isHiddenOrHasHiddenAncestor];
}

void BaseView::Focus() {
  if (nsview_.window && IsFocusable())
    [nsview_.window makeFirstResponder:nsview_];
}

bool BaseView::HasFocus() const {
  if (nsview_.window)
    return nsview_.window.firstResponder == nsview_;
  else
    return false;
}

void BaseView::SetFocusable(bool focusable) {
  NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
  priv->focusable = focusable;
}

bool BaseView::IsFocusable() const {
  return [nsview_ acceptsFirstResponder];
}

void BaseView::SetBackgroundColor(const std::string& color_name) {
  const SkColor color = ParseCSSColor(color_name);
  if (IsNativeView(nsview_))
    [nsview_ setNativeBackgroundColor:color];
  SetBackgroundColorImpl(color);
}

void BaseView::SetVisualEffectMaterial(std::string material) {
  if (!IsNativeView(nsview_) || !vibrant_)
    return;
  NSVisualEffectMaterial m = NSVisualEffectMaterialAppearanceBased;
  if (material == "appearanceBased")
    m = NSVisualEffectMaterialAppearanceBased;
  else if (material == "light")
    m = NSVisualEffectMaterialLight;
  else if (material == "dark")
    m = NSVisualEffectMaterialDark;
  else if (material == "titlebar")
    m = NSVisualEffectMaterialTitlebar;
  static_cast<ElectronNativeVibrantView*>(nsview_).material = m;
}

std::string BaseView::GetVisualEffectMaterial() const {
  if (!IsNativeView(nsview_) || !vibrant_)
    return "appearanceBased";
  NSVisualEffectMaterial material =
      static_cast<ElectronNativeVibrantView*>(nsview_).material;
  if (material == NSVisualEffectMaterialAppearanceBased)
    return "appearanceBased";
  else if (material == NSVisualEffectMaterialLight)
    return "light";
  else if (material == NSVisualEffectMaterialDark)
    return "dark";
  else if (material == NSVisualEffectMaterialTitlebar)
    return "titlebar";
  return "appearanceBased";
}

void BaseView::SetVisualEffectBlendingMode(std::string mode) {
  if (!IsNativeView(nsview_) || !vibrant_)
    return;
  NSVisualEffectBlendingMode m = NSVisualEffectBlendingModeBehindWindow;
  if (mode == "withinWindow") {
    m = NSVisualEffectBlendingModeWithinWindow;
    SetWantsLayer(true);
  } else {
    SetWantsLayer(false);
  }
  static_cast<ElectronNativeVibrantView*>(nsview_).blendingMode = m;
}

std::string BaseView::GetVisualEffectBlendingMode() const {
  if (!IsNativeView(nsview_) || !vibrant_)
    return "behindWindow";
  NSVisualEffectBlendingMode mode =
      static_cast<ElectronNativeVibrantView*>(nsview_).blendingMode;
  if (mode == NSVisualEffectBlendingModeWithinWindow)
    return "withinWindow";
  return "behindWindow";
}

void BaseView::SetBlurTintColorWithSRGB(float r, float g, float b, float a) {
  if (IsNativeView(nsview_) && blurred_) {
    NSColor* color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:a];
    [static_cast<ElectronNativeBlurredView*>(nsview_) setTintColor:color];
  }
}

void BaseView::SetBlurTintColorWithCalibratedWhite(float white,
                                                   float alphaval) {
  if (IsNativeView(nsview_) && blurred_) {
    NSColor* color = [NSColor colorWithCalibratedWhite:white alpha:alphaval];
    [static_cast<ElectronNativeBlurredView*>(nsview_) setTintColor:color];
  }
}

void BaseView::SetBlurTintColorWithGenericGamma22White(float white,
                                                       float alphaval) {
  if (IsNativeView(nsview_) && blurred_) {
    NSColor* color = [NSColor colorWithGenericGamma22White:white
                                                     alpha:alphaval];
    [static_cast<ElectronNativeBlurredView*>(nsview_) setTintColor:color];
  }
}

void BaseView::SetBlurRadius(float radius) {
  if (IsNativeView(nsview_) && blurred_)
    [static_cast<ElectronNativeBlurredView*>(nsview_) setBlurRadius:radius];
}

float BaseView::GetBlurRadius() const {
  if (IsNativeView(nsview_) && blurred_)
    return [static_cast<ElectronNativeBlurredView*>(nsview_) blurRadius];
  return 0.0;
}

void BaseView::SetBlurSaturationFactor(float factor) {
  if (IsNativeView(nsview_) && blurred_) {
    [static_cast<ElectronNativeBlurredView*>(nsview_)
        setSaturationFactor:factor];
  }
}

float BaseView::GetBlurSaturationFactor() const {
  if (IsNativeView(nsview_) && blurred_)
    return [static_cast<ElectronNativeBlurredView*>(nsview_) saturationFactor];
  return 0.0;
}

void BaseView::SetCapture() {
  if (g_captured_view)
    g_captured_view->ReleaseCapture();

  NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
  priv->mouse_capture.reset(new MouseCapture(this));
  g_captured_view = this;
}

void BaseView::ReleaseCapture() {
  if (g_captured_view != this)
    return;

  NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
  priv->mouse_capture.reset();
  g_captured_view = nullptr;
  NotifyCaptureLost();
}

bool BaseView::HasCapture() const {
  return g_captured_view == this;
}

void BaseView::EnableMouseEvents() {
  AddMouseEventHandlerToClass([nsview_ class]);
}

bool BaseView::AreMouseEventsEnabled() const {
  return IsMouseEventHandlerAddedToClass([nsview_ class]);
}

void BaseView::SetMouseTrackingEnabled(bool enable) {
  if (enable) {
    // Install event tracking area.
    [nsview_ enableTracking];
    EnableMouseEvents();
  } else {
    [nsview_ disableTracking];
  }
}

bool BaseView::IsMouseTrackingEnabled() const {
  NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
  return !!(priv->tracking_area);
}

void BaseView::SetRoundedCorners(const RoundedCornersOptions& options) {
  if (@available(macOS 10.13, *)) {
    rounded_corners_options_ = options;
    SetWantsLayer(true);
    nsview_.layer.masksToBounds = YES;
    nsview_.layer.cornerRadius = options.radius;

    CACornerMask mask = 0;
    if (options.top_left)
      mask |= kCALayerMinXMinYCorner;
    if (options.top_right)
      mask |= kCALayerMaxXMinYCorner;
    if (options.bottom_left)
      mask |= kCALayerMinXMaxYCorner;
    if (options.bottom_right)
      mask |= kCALayerMaxXMaxYCorner;
    nsview_.layer.maskedCorners = mask;
  }
}

void BaseView::SetClippingInsets(const ClippingInsetOptions& options) {
  if (@available(macOS 10.13, *)) {
    SetWantsLayer(true);

    CALayer* sublayer = [CALayer layer];
    sublayer.backgroundColor =
        [[NSColor blackColor] colorWithAlphaComponent:1.0].CGColor;

    if (options.left == 0 && options.right == 0 && options.bottom == 0 &&
        options.top == 0) {
      nsview_.layer.mask = nil;
      return;
    }

    int newFrameX = nsview_.frame.origin.x + options.left;
    int newFrameY = nsview_.frame.origin.y + options.bottom;
    int newFrameWidth = nsview_.frame.size.width - options.left - options.right;
    int newFrameHeight =
        nsview_.frame.size.height - options.bottom - options.top;
    sublayer.frame =
        NSMakeRect(newFrameX, newFrameY, newFrameWidth, newFrameHeight);

    nsview_.layer.mask = sublayer;
  }
}

void BaseView::ResetScaling() {
  [nsview_ scaleUnitSquareToSize:[nsview_ convertSize:unitSize fromView:nil]];
}

void BaseView::SetScale(const ScaleAnimationOptions& options) {
  NSSize current_scale = [nsview_ convertSize:unitSize toView:nil];
  NSRect current_frame = [nsview_ frame];

  NSSize new_scale;
  new_scale.width = options.scale_x / current_scale.width;
  new_scale.height = options.scale_y / current_scale.height;
  [nsview_ scaleUnitSquareToSize:new_scale];
  [nsview_ setNeedsDisplay:YES];

  if (options.adjust_frame) {
    // Set the frame to the scaled frame
    nsview_.frame = NSMakeRect(
        current_frame.origin.x, current_frame.origin.y,
        (options.scale_x / current_scale.width) * current_frame.size.width,
        (options.scale_y / current_scale.height) * current_frame.size.height);
  }
  NSRect new_frame = [nsview_ frame];

  float width_diff = current_frame.size.width - new_frame.size.width;
  float height_diff = current_frame.size.height - new_frame.size.height;
  float offset_x = 0.0;
  float offset_y = 0.0;

  if (options.anchor_x_pos == AnchorXPos::kRight)
    offset_x = width_diff;
  else if (options.anchor_x_pos == AnchorXPos::kCenter)
    offset_x = width_diff / 2;
  else if (options.anchor_x_pos == AnchorXPos::kPercentage)
    offset_x = width_diff * (options.anchor_x_percentage / 100.0);

  auto* superview = nsview_.superview;
  if (superview && ![superview isFlipped]) {
    if (options.anchor_y_pos == AnchorYPos::kTop)
      offset_y = height_diff;
    else if (options.anchor_y_pos == AnchorYPos::kCenter)
      offset_y = height_diff / 2;
    else if (options.anchor_y_pos == AnchorYPos::kPercentage)
      offset_y =
          height_diff - height_diff * (options.anchor_y_percentage / 100.0);
  } else {
    if (options.anchor_y_pos == AnchorYPos::kBottom)
      offset_y = height_diff;
    else if (options.anchor_y_pos == AnchorYPos::kCenter)
      offset_y = height_diff / 2;
    else if (options.anchor_y_pos == AnchorYPos::kPercentage)
      offset_y = height_diff * (options.anchor_y_percentage / 100.0);
  }

  if (!options.animation) {
    if (offset_x != 0.0 || offset_y != 0.0)
      nsview_.frame = CGRectOffset(nsview_.frame, offset_x, offset_y);
    return;
  }

  CAMediaTimingFunction* timing_function_scale = nil;
  CAMediaTimingFunction* timing_function_tran = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function_scale =
        [CAMediaTimingFunction functionWithName:timing_function_name];
    if (offset_x != 0.0 || offset_y != 0.0) {
      timing_function_tran =
          [CAMediaTimingFunction functionWithName:timing_function_name];
    }
  } else {
    timing_function_scale =
          [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
    if (offset_x != 0.0 || offset_y != 0.0) {
      timing_function_tran =
            [[CAMediaTimingFunction alloc] initWithControlPoints:
                                                             options.cx1:options.cy1:options.cx2:options.cy2];
    }
  }

  // Create the scale animation
  CABasicAnimation* animation = [CABasicAnimation animation];
  animation.duration = options.duration;
  animation.timingFunction = timing_function_scale;
  animation.fromValue =
      @(CATransform3DMakeScale(current_scale.width, current_scale.height, 1.0));
  animation.toValue =
      @(CATransform3DMakeScale(options.scale_x, options.scale_y, 1.0));
  // Trigger the scale animation
  [nsview_.layer addAnimation:animation forKey:@"transform"];

  if (offset_x != 0.0 || offset_y != 0.0) {
    // Add a simultaneous translation animation
    [NSAnimationContext
        runAnimationGroup:^(NSAnimationContext* context) {
          // Match the configuration of the scale animation
          context.duration = options.duration;
          context.timingFunction = timing_function_tran;
          nsview_.animator.frame =
              CGRectOffset(nsview_.frame, offset_x, offset_y);
        }
        completionHandler:^{
        }];
  }
}

float BaseView::GetScaleX() const {
  NSSize size = [nsview_ convertSize:unitSize toView:nil];
  return size.width;
}

float BaseView::GetScaleY() const {
  NSSize size = [nsview_ convertSize:unitSize toView:nil];
  return size.height;
}

void BaseView::SetOpacity(const double opacity, gin::Arguments* args) {
  AnimationOptions options;
  args->GetNext(&options);

  const double bounded_opacity = base::clamp(opacity, 0.0, 1.0);
  if (!options.animation) {
    [nsview_ setAlphaValue:bounded_opacity];
    return;
  }

  CAMediaTimingFunction* timing_function = nil;
  if (!options.use_control_points) {
    CAMediaTimingFunctionName timing_function_name =
        kCAMediaTimingFunctionLinear;
    if (options.timing_function == TimingFunction::kLinear)
      timing_function_name = kCAMediaTimingFunctionLinear;
    else if (options.timing_function == TimingFunction::kEaseIn)
      timing_function_name = kCAMediaTimingFunctionEaseIn;
    else if (options.timing_function == TimingFunction::kEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseOut;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      timing_function_name = kCAMediaTimingFunctionEaseInEaseOut;
    else if (options.timing_function == TimingFunction::kDefault)
      timing_function_name = kCAMediaTimingFunctionDefault;
    timing_function =
        [CAMediaTimingFunction functionWithName:timing_function_name];
  } else {
    timing_function =
        [[CAMediaTimingFunction alloc] initWithControlPoints:options.cx1:options.cy1:options.cx2:options.cy2];
  }

  [NSAnimationContext
      runAnimationGroup:^(NSAnimationContext* context) {
        context.duration = options.duration;
        context.timingFunction = timing_function;
        // Trigger the animation
        nsview_.animator.alphaValue = bounded_opacity;
      }
      completionHandler:^{
      }];
}

double BaseView::GetOpacity() const {
  return [nsview_ alphaValue];
}

void BaseView::RearrangeChildViews() {
  if (api_children_.size() == 0)
    return;

  [CATransaction begin];
  [CATransaction setDisableActions:YES];

  std::sort(api_children_.begin(), api_children_.end(),
            [](auto* a, auto* b) { return a->GetZIndex() < b->GetZIndex(); });

  auto begin = api_children_.begin();
  auto* first = *begin;
  begin++;

  for (auto it = begin; it != api_children_.end(); it++) {
    auto* second = *it;

    auto* firstNSView = first->GetNSView();
    auto* secondNSView = second->GetNSView();

    [nsview_ addSubview:secondNSView
             positioned:NSWindowAbove
             relativeTo:firstNSView];

    first = second;
  }

  [CATransaction commit];
}

std::vector<v8::Local<v8::Value>> BaseView::GetNativelyRearrangedViews() const {
  std::vector<v8::Local<v8::Value>> ret;

  base::scoped_nsobject<NSArray> subviews([[nsview_ subviews] copy]);
  for (NSView* child in subviews.get()) {
    auto child_iter = std::find_if(api_children_.begin(), api_children_.end(),
                                   [child](const auto* api_child) {
                                     return child == api_child->GetNSView();
                                   });
    DCHECK(child_iter != api_children_.end());
    if (child_iter != api_children_.end()) {
      ret.push_back(
          v8::Local<v8::Value>::New(isolate(), (*child_iter)->GetWrapper()));
    }
  }

  return ret;
}

void BaseView::SetView(NSView* view) {
  nsview_ = view;

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

void BaseView::DestroyView() {
  if (!nsview_)
    return;
  if (IsNativeView(nsview_)) {
    // Release all hooks before destroying the view.
    [nsview_ disableTracking];
    ReleaseCapture();
    // The view may be referenced after this class gets destroyed.
    NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
    priv->shell = nullptr;
  }
  [nsview_ release];
  nsview_ = nullptr;
}

void BaseView::SetWantsLayer(bool wants) {
  [nsview_ nativeViewPrivate]->wants_layer = wants;
  [nsview_ setWantsLayer:wants];
}

bool BaseView::WantsLayer() const {
  return [nsview_ wantsLayer];
}

void BaseView::AddChildViewImpl(BaseView* view) {
  if (IsScaled() && [[nsview_ subviews] count] == 0) {
LOG(ERROR) << "@@if (IsScaled() && [[nsview_ subviews] count] == 0) {";
    [(ElectronScaleRotateFlipView *) nsview_ setContentView:view->GetNSView()];
  } else {
    [nsview_ addSubview:view->GetNSView()];
  }
  NativeViewPrivate* priv = [nsview_ nativeViewPrivate];
  if (priv->wants_layer_infected) {
    [view->GetNSView() setWantsLayer:YES];
  } else {
    if (IsNativeView(view->GetNSView()) &&
        [view->GetNSView() nativeViewPrivate]->wants_layer_infected) {
      priv->wants_layer_infected = true;
      SetWantsLayer(true);
      for (int i = 0; i < ChildCount(); ++i)
        [api_children_[i]->GetNSView() setWantsLayer:YES];
    }
  }
}

void BaseView::RemoveChildViewImpl(BaseView* view) {
  NSView* nsview = view->GetNSView();
  [nsview removeFromSuperview];
  if (IsNativeView(nsview))
    [nsview setWantsLayer:[nsview nativeViewPrivate]->wants_layer];
  else
    [nsview setWantsLayer:NO];
}

void BaseView::NotifyCaptureLost() {
  Emit("capture-lost");
}

}  // namespace api

}  // namespace electron
