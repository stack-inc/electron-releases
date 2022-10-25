// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_scroll_view.h"

#include <cmath>

#include "base/strings/string_util.h"
#include "shell/browser/browser.h"
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
  electron::api::ScrollView* shell_;
  BOOL scroll_events_enabled_;
  BOOL scroll_wheel_swapped_;
  double scroll_wheel_factor_;
  NSSize content_size_;
}
- (id)initWithShell:(electron::api::ScrollView*)shell;
- (void)setScrollEventsEnabled:(BOOL)enable;
- (BOOL)scrollEventsEnabled;
- (void)setScrollWheelSwapped:(BOOL)swap;
- (BOOL)scrollWheelSwapped;
- (void)setScrollWheelFactor:(double)factor;
- (double)scrollWheelFactor;
- (void)setContentSize:(NSSize)size;
@end

@implementation ElectronNativeScrollView

- (id)initWithShell:(electron::api::ScrollView*)shell {
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
  shell_->NotifyDidScroll();
}

- (void)onWillStartLiveScroll:(NSNotification*)notification {
  shell_->NotifyWillStartLiveScroll();
}

- (void)onDidLiveScroll:(NSNotification*)notification {
  shell_->NotifyDidLiveScroll();
}

- (void)onDidEndLiveScroll:(NSNotification*)notification {
  shell_->NotifyDidEndLiveScroll();
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
}

#if 0
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
#endif

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
    shell_->NotifyScrollWheel(event.subtype == NSEventSubtypeMouseEvent,
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

namespace electron::api {

void ScrollView::SetContentSizeMac(const gfx::Size& size) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  NSSize content_size = size.ToCGSize();
  [scroll setContentSize:content_size];
  [scroll.documentView setFrameSize:content_size];
}

void ScrollView::SetHorizontalScrollBarModeMac(std::string mode) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (mode == "disabled") {
    [scroll setHasHorizontalScroller:NO];
  } else if (mode == "enabled-but-hidden") {
    [scroll setHasHorizontalScroller:YES];
    [[scroll horizontalScroller] setAlphaValue:0];
  } else {
    [scroll setHasHorizontalScroller:YES];
  }
}

std::string ScrollView::GetHorizontalScrollBarModeMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (scroll.hasHorizontalScroller) {
    if ([[scroll horizontalScroller] alphaValue] != 0.0)
      return "enabled";
    else
      return "enabled-but-hidden";
  }
  return "disabled";
}

void ScrollView::SetVerticalScrollBarModeMac(std::string mode) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (mode == "disabled") {
    [scroll setHasVerticalScroller:NO];
  } else if (mode == "enabled-but-hidden") {
    [scroll setHasVerticalScroller:YES];
    [[scroll verticalScroller] setAlphaValue:0];
  } else {
    [scroll setHasVerticalScroller:YES];
  }
}

std::string ScrollView::GetVerticalScrollBarModeMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (scroll.hasVerticalScroller) {
    if ([[scroll verticalScroller] alphaValue] != 0.0)
      return "enabled";
    else
      return "enabled-but-hidden";
  }
  return "disabled";
}

void ScrollView::SetScrollWheelSwappedMac(bool swap) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  [scroll setScrollWheelSwapped:swap ? YES : NO];
}

bool ScrollView::IsScrollWheelSwappedMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  return [scroll scrollWheelSwapped];
}

void ScrollView::SetScrollEventsEnabledMac(bool enable) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  [scroll setScrollEventsEnabled:enable ? YES : NO];
}

bool ScrollView::IsScrollEventsEnabledMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  return [scroll scrollEventsEnabled];
}

void ScrollView::SetHorizontalScrollElasticityMac(std::string elasticity) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (elasticity == "none")
    scroll.horizontalScrollElasticity = NSScrollElasticityNone;
  else if (elasticity == "allowed")
    scroll.horizontalScrollElasticity = NSScrollElasticityAllowed;
  else
    scroll.horizontalScrollElasticity = NSScrollElasticityAutomatic;
}

std::string ScrollView::GetHorizontalScrollElasticityMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  switch (scroll.horizontalScrollElasticity) {
    case NSScrollElasticityNone:
      return "none";
    case NSScrollElasticityAllowed:
      return "allowed";
    default:
      return "automatic";
  }
}

void ScrollView::SetVerticalScrollElasticityMac(std::string elasticity) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  if (elasticity == "none")
    scroll.verticalScrollElasticity = NSScrollElasticityNone;
  else if (elasticity == "allowed")
    scroll.verticalScrollElasticity = NSScrollElasticityAllowed;
  else
    scroll.verticalScrollElasticity = NSScrollElasticityAutomatic;
}

std::string ScrollView::GetVerticalScrollElasticityMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  switch (scroll.verticalScrollElasticity) {
    case NSScrollElasticityNone:
      return "none";
    case NSScrollElasticityAllowed:
      return "allowed";
    default:
      return "automatic";
  }
}

gfx::Point ScrollView::GetScrollPositionMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  NSPoint point = scroll.contentView.bounds.origin;
  if (![scroll.documentView isFlipped]) {
    point.y = NSHeight(scroll.documentView.bounds) -
              NSHeight(scroll.contentView.bounds) - point.y;
  }
  return gfx::Point(point.x, point.y);
}

gfx::Point ScrollView::GetMaximumScrollPositionMac() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  NSRect docBounds = scroll.documentView.bounds;
  NSRect clipBounds = scroll.contentView.bounds;
  return gfx::Point(std::max(0.0, NSMaxX(docBounds) - NSWidth(clipBounds)),
                    std::max(0.0, NSMaxY(docBounds) - NSHeight(clipBounds)));
}

void ScrollView::ScrollToPoint(gfx::Point point,
                               const AnimationOptions& options) {
  if (Browser::Get()->IsViewsUsage())
    return;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());

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

void ScrollView::ScrollPointToCenter(gfx::Point point,
                                     const AnimationOptions& options) {
  if (Browser::Get()->IsViewsUsage())
    return;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());

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

void ScrollView::SetOverlayScrollbar(bool overlay) {
  if (Browser::Get()->IsViewsUsage())
    return;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  scroll.scrollerStyle =
      overlay ? NSScrollerStyleOverlay : NSScrollerStyleLegacy;
}

bool ScrollView::IsOverlayScrollbar() const {
  if (Browser::Get()->IsViewsUsage())
    return false;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  return scroll.scrollerStyle == NSScrollerStyleOverlay;
}

void ScrollView::SetScrollWheelFactor(double factor) {
  if (Browser::Get()->IsViewsUsage())
    return;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  [scroll setScrollWheelFactor:factor];
}

double ScrollView::GetScrollWheelFactor() const {
  if (Browser::Get()->IsViewsUsage())
    return 1.0;
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  return [scroll scrollWheelFactor];
}

void ScrollView::CreateScrollViewMac() {
  auto* scroll = [[ElectronNativeScrollView alloc] initWithShell:this];
  scroll.drawsBackground = NO;
  if (scroll.scrollerStyle == NSScrollerStyleOverlay) {
    scroll.hasHorizontalScroller = YES;
    scroll.hasVerticalScroller = YES;
  }
  [scroll.contentView setCopiesOnScroll:NO];
  [scroll.contentView setAutoresizesSubviews:NO];
  SetView(scroll);
}

void ScrollView::SetContentViewImplMac(BaseView* view) {
  auto* scroll = static_cast<NSScrollView*>(GetNSView());
  scroll.documentView = view->GetNSView();
  [scroll.documentView setAutoresizingMask:NSViewNotSizable];
}

void ScrollView::ResetCurrentContentViewImplMac() {
  auto* scroll = static_cast<NSScrollView*>(GetNSView());
  scroll.documentView = nil;
}

void ScrollView::SetScrollPositionImplMac(
    gfx::Point point,
    base::OnceCallback<void(std::string)> callback) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNSView());
  int vertical = point.y();
  if (![scroll.documentView isFlipped])
    vertical = NSHeight(scroll.documentView.bounds) - vertical;
  [scroll.documentView scrollPoint:NSMakePoint(point.x(), vertical)];

  std::move(callback).Run(std::string());
}

void ScrollView::NotifyWillStartLiveScroll() {
  Emit("will-start-live-scroll");
}

void ScrollView::NotifyDidLiveScroll() {
  Emit("did-live-scroll");
}

void ScrollView::NotifyDidEndLiveScroll() {
  Emit("did-end-live-scroll");
}

void ScrollView::NotifyScrollWheel(bool mouse_event,
                                   float scrolling_delta_x,
                                   float scrolling_delta_y,
                                   std::string phase,
                                   std::string momentum_phase) {
  Emit("scroll-wheel", mouse_event, scrolling_delta_x, scrolling_delta_y, phase,
       momentum_phase);
}

}  // namespace electron::api
