#include "shell/browser/ui/native_scroll_view.h"

#include "base/strings/string_util.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/common/gin_helper/dictionary.h"

@interface ElectronNativeScrollView
    : NSScrollView <ElectronNativeViewProtocol> {
 @private
  electron::NativeViewPrivate private_;
  electron::NativeScrollView* shell_;
  NSSize content_size_;
}
- (id)initWithShell:(electron::NativeScrollView*)shell;
- (void)setContentSize:(NSSize)size;
@end

@implementation ElectronNativeScrollView

- (id)initWithShell:(electron::NativeScrollView*)shell {
  if ((self = [super init])) {
    shell_ = shell;
  }
  return self;
}

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [super dealloc];
}

- (void)setContentSize:(NSSize)size {
  content_size_ = size;
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
  NSSize parent_size = [self frame].size;
  NSSize content_size = content_size_;
  if (content_size.width < parent_size.width)
    content_size.width = parent_size.width;
  if (content_size.height < parent_size.height)
    content_size.height = parent_size.height;
  [self.documentView setFrameSize:content_size];
  [super resizeSubviewsWithOldSize:oldBoundsSize];
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

void NativeScrollView::InitScrollView() {
  auto* scroll = [[ElectronNativeScrollView alloc] initWithShell:this];
  scroll.drawsBackground = NO;
  if (scroll.scrollerStyle == NSScrollerStyleOverlay) {
    scroll.hasHorizontalScroller = YES;
    scroll.hasVerticalScroller = YES;
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
  return gfx::Point(NSMaxX(docBounds) - NSWidth(clipBounds),
                    NSMaxY(docBounds) - NSHeight(clipBounds));
}

void NativeScrollView::ScrollToPoint(gfx::Point point,
                                     const gin_helper::Dictionary& options) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  float duration = 0.4;
  options.Get("duration", &duration);
  std::string tfunction_name;
  if (options.Get("timingFunction", &tfunction_name)) {
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
  }

  NSAnimationCurve animation_curve = NSAnimationEaseInOut;
  if (tfunction_name == "linear") {
    animation_curve = NSAnimationLinear;
  } else if (tfunction_name == "easein") {
    animation_curve = NSAnimationEaseIn;
  } else if (tfunction_name == "easeout") {
    animation_curve = NSAnimationEaseOut;
  } else if (tfunction_name == "easeineaseout") {
    animation_curve = NSAnimationEaseInOut;
  }

  [ScrollViewAnimation animatedScrollToPoint:NSMakePoint(point.x(), point.y())
                                inScrollView:scroll
                                 forDuration:duration
                          withAnimationCurve:animation_curve];
}

void NativeScrollView::ScrollPointToCenter(
    gfx::Point point,
    const gin_helper::Dictionary& options) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  float duration = 0.4;
  options.Get("duration", &duration);
  std::string tfunction_name;
  if (options.Get("timingFunction", &tfunction_name)) {
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
  }

  NSAnimationCurve animation_curve = NSAnimationEaseInOut;
  if (tfunction_name == "linear") {
    animation_curve = NSAnimationLinear;
  } else if (tfunction_name == "easein") {
    animation_curve = NSAnimationEaseIn;
  } else if (tfunction_name == "easeout") {
    animation_curve = NSAnimationEaseOut;
  } else if (tfunction_name == "easeineaseout") {
    animation_curve = NSAnimationEaseInOut;
  }

  [ScrollViewAnimation
      animatedScrollPointToCenter:NSMakePoint(point.x(), point.y())
                     inScrollView:scroll
                      forDuration:duration
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
  scroll.hasHorizontalScroller = mode != ScrollBarMode::kDisabled;
}

ScrollBarMode NativeScrollView::GetHorizontalScrollBarMode() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return scroll.hasHorizontalScroller ? ScrollBarMode::kEnabled
                                      : ScrollBarMode::kDisabled;
}

void NativeScrollView::SetVerticalScrollBarMode(ScrollBarMode mode) {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  scroll.hasVerticalScroller = mode != ScrollBarMode::kDisabled;
}

ScrollBarMode NativeScrollView::GetVerticalScrollBarMode() const {
  auto* scroll = static_cast<ElectronNativeScrollView*>(GetNative());
  return scroll.hasVerticalScroller ? ScrollBarMode::kEnabled
                                    : ScrollBarMode::kDisabled;
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

void NativeScrollView::UpdateDraggableRegions() {
  if (content_view_.get())
    content_view_->UpdateDraggableRegions();
}

}  // namespace electron
