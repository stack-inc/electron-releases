#include "shell/browser/ui/native_scroll_view.h"

#include <cmath>

#include "base/strings/string_util.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"

namespace {

std::string phaseToString(NSEventPhase phase) {
  switch (phase) {
    case NSEventPhaseNone:
      return "none";
    case NSEventPhaseBegan:
      return "began";
    case NSEventPhaseStationary:
      return "stationary";
    case NSEventPhaseChanged:
      return "changed";
    case NSEventPhaseEnded:
      return "ended";
    case NSEventPhaseCancelled:
      return "cancelled";
    case NSEventPhaseMayBegin:
      return "mayBegin";
    default:
      return "unknown";
  }
}

}  // namespace

@interface ElectronNativeScrollView
    : NSScrollView <ElectronNativeViewProtocol> {
 @private
  electron::NativeViewPrivate private_;
  electron::NativeScrollView* shell_;
  BOOL scroll_events_enabled_;
  BOOL scroll_wheel_swapped_;
  double scroll_wheel_factor_;
  NSSize content_size_;
}
- (id)initWithShell:(electron::NativeScrollView*)shell;
- (void)setScrollEventsEnabled:(BOOL)enable;
- (BOOL)scrollEventsEnabled;
- (void)setScrollWheelSwapped:(BOOL)swap;
- (BOOL)scrollWheelSwapped;
- (void)setScrollWheelFactor:(double)factor;
- (double)scrollWheelFactor;
- (void)setContentSize:(NSSize)size;
@end

@implementation ElectronNativeScrollView

- (id)initWithShell:(electron::NativeScrollView*)shell {
  if ((self = [super init])) {
    shell_ = shell;
    scroll_events_enabled_ = NO;
    scroll_wheel_swapped_ = NO;
    scroll_wheel_factor_ = 1.0;
  }
  return self;
}

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [[NSNotificationCenter defaultCenter] removeObserver:self];
  [super dealloc];
}

- (void)setScrollEventsEnabled:(BOOL)enable {
  if (scroll_events_enabled_ == enable)
    return;
  scroll_events_enabled_ = enable;
  NSView* contentView = self.contentView;
  if (enable) {
    [contentView setPostsBoundsChangedNotifications:YES];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(onDidScroll:)
               name:NSViewBoundsDidChangeNotification
             object:contentView];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(onWillStartLiveScroll:)
               name:NSScrollViewWillStartLiveScrollNotification
             object:self];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(onDidLiveScroll:)
               name:NSScrollViewDidLiveScrollNotification
             object:self];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(onDidEndLiveScroll:)
               name:NSScrollViewDidEndLiveScrollNotification
             object:self];
  } else {
    [contentView setPostsBoundsChangedNotifications:NO];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
  }
}

- (BOOL)scrollEventsEnabled {
  return scroll_events_enabled_;
}

- (void)setScrollWheelSwapped:(BOOL)swap {
  scroll_wheel_swapped_ = swap;
}

- (BOOL)scrollWheelSwapped {
  return scroll_wheel_swapped_;
}

- (void)setScrollWheelFactor:(double)factor {
  scroll_wheel_factor_ = factor;
}

- (double)scrollWheelFactor {
  return scroll_wheel_factor_;
}

- (void)setContentSize:(NSSize)size {
  content_size_ = size;
}

- (void)onDidScroll:(NSNotification*)notification {
  shell_->NotifyDidScroll(shell_);
}

- (void)onWillStartLiveScroll:(NSNotification*)notification {
  shell_->NotifyWillStartLiveScroll(shell_);
}

- (void)onDidLiveScroll:(NSNotification*)notification {
  shell_->NotifyDidLiveScroll(shell_);
}

- (void)onDidEndLiveScroll:(NSNotification*)notification {
  shell_->NotifyDidEndLiveScroll(shell_);
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
  // Automatically resize the content view when ScrollView is larger than the
  // content size.
  NSSize parent_size = [self frame].size;
  NSSize content_size = content_size_;
  if (content_size.width < parent_size.width)
    content_size.width = parent_size.width;
  if (content_size.height < parent_size.height)
    content_size.height = parent_size.height;
  [self.documentView setFrameSize:content_size];
  [super resizeSubviewsWithOldSize:oldBoundsSize];
}

- (void)scrollWheel:(NSEvent*)event {
  if (scroll_wheel_swapped_ && event.subtype == NSEventSubtypeMouseEvent) {
    CGEventRef cgEvent = CGEventCreateCopy(event.CGEvent);
    if (cgEvent) {
      CGEventSetDoubleValueField(cgEvent, kCGScrollWheelEventDeltaAxis1, 0.0);
      CGEventSetDoubleValueField(
          cgEvent, kCGScrollWheelEventDeltaAxis2,
          static_cast<double>(event.scrollingDeltaY) * scroll_wheel_factor_);
      NSEvent* newEvent = [NSEvent eventWithCGEvent:cgEvent];
      if (newEvent) {
        [super scrollWheel:newEvent];
      }
    } else {
      [super scrollWheel:event];
    }
  } else if (scroll_wheel_swapped_ && std::abs(event.scrollingDeltaY) >
                                          std::abs(event.scrollingDeltaX)) {
    CGEventRef cgEvent = CGEventCreateCopy(event.CGEvent);
    if (cgEvent) {
      CGEventSetDoubleValueField(cgEvent, kCGScrollWheelEventDeltaAxis1, 0.0);
      CGEventSetDoubleValueField(
          cgEvent, kCGScrollWheelEventDeltaAxis2,
          static_cast<double>(event.scrollingDeltaY) * scroll_wheel_factor_);
      NSEvent* newEvent = [NSEvent eventWithCGEvent:cgEvent];
      if (newEvent) {
        [super scrollWheel:newEvent];
      }
    } else {
      [super scrollWheel:event];
    }
  } else {
    [super scrollWheel:event];
  }

  if (scroll_events_enabled_) {
    shell_->NotifyScrollWheel(shell_, event.subtype == NSEventSubtypeMouseEvent,
                              event.scrollingDeltaX, event.scrollingDeltaY,
                              phaseToString(event.phase),
                              phaseToString(event.momentumPhase));
  }
}

@end

@interface ScrollViewAnimation : NSAnimation
@property(nonatomic, strong) NSScrollView* scrollView;
@property(nonatomic) NSPoint originPoint;
@property(nonatomic) NSPoint targetPoint;

+ (void)animatedScrollPointToCenter:(NSPoint)targetPoint
                       inScrollView:(NSScrollView*)scrollView
                        forDuration:(float)duration
                 withAnimationCurve:(NSAnimationCurve)animationCurve;
+ (void)animatedScrollToPoint:(NSPoint)targetPoint
                 inScrollView:(NSScrollView*)scrollView
                  forDuration:(float)duration
           withAnimationCurve:(NSAnimationCurve)animationCurve;
@end

@implementation ScrollViewAnimation

@synthesize scrollView;
@synthesize originPoint;
@synthesize targetPoint;

+ (void)animatedScrollPointToCenter:(NSPoint)targetPoint
                       inScrollView:(NSScrollView*)scrollView
                        forDuration:(float)duration
                 withAnimationCurve:(NSAnimationCurve)animationCurve {
  NSRect visibleRect = [scrollView documentVisibleRect];
  targetPoint = NSMakePoint(targetPoint.x - (NSWidth(visibleRect) / 2),
                            targetPoint.y - (NSHeight(visibleRect) / 2));
  [self animatedScrollToPoint:targetPoint
                 inScrollView:scrollView
                  forDuration:duration
           withAnimationCurve:animationCurve];
}

+ (void)animatedScrollToPoint:(NSPoint)targetPoint
                 inScrollView:(NSScrollView*)scrollView
                  forDuration:(float)duration
           withAnimationCurve:(NSAnimationCurve)animationCurve {
  ScrollViewAnimation* animation =
      [[ScrollViewAnimation alloc] initWithDuration:duration
                                     animationCurve:animationCurve];
  animation.scrollView = scrollView;
  animation.originPoint = [scrollView documentVisibleRect].origin;
  animation.targetPoint = targetPoint;

  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
    [animation startAnimation];
  });
}

- (void)setCurrentProgress:(NSAnimationProgress)progress {
  typedef float (^MyAnimationCurveBlock)(float, float, float);
  MyAnimationCurveBlock cubicEaseInOut =
      ^float(float t, float start, float end) {
        t *= 2.;
        if (t < 1.)
          return end / 2 * t * t * t + start - 1.f;
        t -= 2;
        return end / 2 * (t * t * t + 2) + start - 1.f;
      };

  dispatch_sync(dispatch_get_main_queue(), ^{
    NSPoint progressPoint = self.originPoint;
    progressPoint.x +=
        cubicEaseInOut(progress, 0, self.targetPoint.x - self.originPoint.x);
    progressPoint.y +=
        cubicEaseInOut(progress, 0, self.targetPoint.y - self.originPoint.y);

    [[self.scrollView documentView] scrollPoint:progressPoint];
    [self.scrollView displayIfNeeded];
  });
}

@end

namespace electron {

void NativeScrollView::InitScrollView(
    absl::optional<ScrollBarMode> horizontal_mode,
    absl::optional<ScrollBarMode> vertical_mode) {
  auto* scroll = [[ElectronNativeScrollView alloc] initWithShell:this];
  scroll.drawsBackground = NO;
  if (scroll.scrollerStyle == NSScrollerStyleOverlay) {
    if (horizontal_mode)
      scroll.hasHorizontalScroller =
          (horizontal_mode.value() == ScrollBarMode::kEnabled) ? YES : NO;
    else
      scroll.hasHorizontalScroller = YES;
    if (vertical_mode)
      scroll.hasVerticalScroller =
          (vertical_mode.value() == ScrollBarMode::kEnabled) ? YES : NO;
    else
      scroll.hasVerticalScroller = YES;
  } else {
    if (horizontal_mode)
      scroll.hasHorizontalScroller =
          (horizontal_mode.value() == ScrollBarMode::kEnabled) ? YES : NO;
    if (vertical_mode)
      scroll.hasVerticalScroller =
          (vertical_mode.value() == ScrollBarMode::kEnabled) ? YES : NO;
  }
  [scroll.contentView setCopiesOnScroll:NO];
  SetNativeView(scroll);
}

void NativeScrollView::SetContentViewImpl(NativeView* view) {
  auto* scroll = static_cast<NSScrollView*>(GetNative());
  scroll.documentView = view->GetNative();
}

void NativeScrollView::DetachChildViewImpl() {
  auto* scroll = static_cast<NSScrollView*>(GetNative());
  scroll.documentView = nil;
}

void NativeScrollView::SetContentSize(const gfx::Size& size) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  NSSize content_size = size.ToCGSize();
  [scroll setContentSize:content_size];
  [scroll.documentView setFrameSize:content_size];
}

void NativeScrollView::SetScrollPosition(gfx::Point point) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  int vertical = point.y();
  if (![scroll.documentView isFlipped])
    vertical = NSHeight(scroll.documentView.bounds) - vertical;
  [scroll.documentView scrollPoint:NSMakePoint(point.x(), vertical)];
}

gfx::Point NativeScrollView::GetScrollPosition() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  NSPoint point = scroll.contentView.bounds.origin;
  if (![scroll.documentView isFlipped]) {
    point.y = NSHeight(scroll.documentView.bounds) -
              NSHeight(scroll.contentView.bounds) - point.y;
  }
  return gfx::Point(point.x, point.y);
}

gfx::Point NativeScrollView::GetMaximumScrollPosition() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  NSRect docBounds = scroll.documentView.bounds;
  NSRect clipBounds = scroll.contentView.bounds;
  return gfx::Point(std::max(0.0, NSMaxX(docBounds) - NSWidth(clipBounds)),
                    std::max(0.0, NSMaxY(docBounds) - NSHeight(clipBounds)));
}

void NativeScrollView::ScrollToPoint(gfx::Point point,
                                     const AnimationOptions& options) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());

  NSAnimationCurve animation_curve = NSAnimationEaseInOut;
  if (options.timing_function == TimingFunction::kLinear)
    animation_curve = NSAnimationLinear;
  else if (options.timing_function == TimingFunction::kEaseIn)
    animation_curve = NSAnimationEaseIn;
  else if (options.timing_function == TimingFunction::kEaseOut)
    animation_curve = NSAnimationEaseOut;
  else if (options.timing_function == TimingFunction::kEaseInEaseOut)
    animation_curve = NSAnimationEaseInOut;

  [ScrollViewAnimation animatedScrollToPoint:NSMakePoint(point.x(), point.y())
                                inScrollView:scroll
                                 forDuration:options.duration
                          withAnimationCurve:animation_curve];
}

void NativeScrollView::ScrollPointToCenter(gfx::Point point,
                                           const AnimationOptions& options) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());

  NSAnimationCurve animation_curve = NSAnimationEaseInOut;
  if (options.timing_function == TimingFunction::kLinear)
    animation_curve = NSAnimationLinear;
  else if (options.timing_function == TimingFunction::kEaseIn)
    animation_curve = NSAnimationEaseIn;
  else if (options.timing_function == TimingFunction::kEaseOut)
    animation_curve = NSAnimationEaseOut;
  else if (options.timing_function == TimingFunction::kEaseInEaseOut)
    animation_curve = NSAnimationEaseInOut;

  [ScrollViewAnimation
      animatedScrollPointToCenter:NSMakePoint(point.x(), point.y())
                     inScrollView:scroll
                      forDuration:options.duration
               withAnimationCurve:animation_curve];
}

void NativeScrollView::SetOverlayScrollbar(bool overlay) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  scroll.scrollerStyle =
      overlay ? NSScrollerStyleOverlay : NSScrollerStyleLegacy;
}

bool NativeScrollView::IsOverlayScrollbar() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return scroll.scrollerStyle == NSScrollerStyleOverlay;
}

void NativeScrollView::SetHorizontalScrollBarMode(ScrollBarMode mode) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  switch (mode) {
    case ScrollBarMode::kDisabled:
      [scroll setHasHorizontalScroller:NO];
      break;
    case ScrollBarMode::kHiddenButEnabled:
      [scroll setHasHorizontalScroller:YES];
      [[scroll horizontalScroller] setAlphaValue:0];
      break;
    case ScrollBarMode::kEnabled:
      [scroll setHasHorizontalScroller:YES];
  }
}

ScrollBarMode NativeScrollView::GetHorizontalScrollBarMode() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  if (scroll.hasHorizontalScroller) {
    if ([[scroll horizontalScroller] alphaValue] != 0.0)
      return ScrollBarMode::kEnabled;
    else
      return ScrollBarMode::kHiddenButEnabled;
  }
  return ScrollBarMode::kDisabled;
}

void NativeScrollView::SetVerticalScrollBarMode(ScrollBarMode mode) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  switch (mode) {
    case ScrollBarMode::kDisabled:
      [scroll setHasVerticalScroller:NO];
      break;
    case ScrollBarMode::kHiddenButEnabled:
      [scroll setHasVerticalScroller:YES];
      [[scroll verticalScroller] setAlphaValue:0];
      break;
    case ScrollBarMode::kEnabled:
      [scroll setHasVerticalScroller:YES];
  }
}

ScrollBarMode NativeScrollView::GetVerticalScrollBarMode() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  if (scroll.hasVerticalScroller) {
    if ([[scroll verticalScroller] alphaValue] != 0.0)
      return ScrollBarMode::kEnabled;
    else
      return ScrollBarMode::kHiddenButEnabled;
  }
  return ScrollBarMode::kDisabled;
}

void NativeScrollView::SetHorizontalScrollElasticity(
    ScrollElasticity elasticity) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  scroll.horizontalScrollElasticity =
      static_cast<NSScrollElasticity>(elasticity);
}

ScrollElasticity NativeScrollView::GetHorizontalScrollElasticity() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return static_cast<ScrollElasticity>(scroll.horizontalScrollElasticity);
}

void NativeScrollView::SetVerticalScrollElasticity(
    ScrollElasticity elasticity) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  scroll.verticalScrollElasticity = static_cast<NSScrollElasticity>(elasticity);
}

ScrollElasticity NativeScrollView::GetVerticalScrollElasticity() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return static_cast<ScrollElasticity>(scroll.verticalScrollElasticity);
}

void NativeScrollView::SetScrollEventsEnabled(bool enable) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  [scroll setScrollEventsEnabled:enable ? YES : NO];
}

bool NativeScrollView::IsScrollEventsEnabled() {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return [scroll scrollEventsEnabled];
}

void NativeScrollView::SetScrollWheelSwapped(bool swap) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  [scroll setScrollWheelSwapped:swap ? YES : NO];
}

bool NativeScrollView::IsScrollWheelSwapped() {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return [scroll scrollWheelSwapped];
}

void NativeScrollView::SetScrollWheelFactor(double factor) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  [scroll setScrollWheelFactor:factor];
}

double NativeScrollView::GetScrollWheelFactor() {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return [scroll scrollWheelFactor];
}

void NativeScrollView::UpdateDraggableRegions() {
  if (content_view_.get())
    content_view_->UpdateDraggableRegions();
}

}  // namespace electron
