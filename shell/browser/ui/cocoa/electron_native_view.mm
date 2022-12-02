#include "shell/browser/ui/cocoa/electron_native_view.h"

#include <objc/objc-runtime.h>

#include "base/logging.h"
#include "base/mac/mac_util.h"
#include "base/mac/scoped_cftyperef.h"
#include "shell/browser/api/electron_api_base_view.h"
#include "shell/browser/native_window_mac.h"
#include "shell/browser/ui/cocoa/electron_ns_window.h"
#include "skia/ext/skia_utils_mac.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"

typedef struct CGContext* CGContextRef;

@implementation ElectronNativeView

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [super dealloc];
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
  background_color_ = absl::make_optional(color);
  [self setNeedsDisplay:YES];
}

- (BOOL)isFlipped {
  return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
  if (!background_color_.has_value())
    return;

  SkColor background_color = background_color_.value();
  gfx::RectF dirty(dirtyRect);
  CGContextRef context = reinterpret_cast<CGContextRef>(
      [[NSGraphicsContext currentContext] graphicsPort]);
  CGContextSetRGBStrokeColor(context, SkColorGetR(background_color) / 255.0f,
                             SkColorGetG(background_color) / 255.0f,
                             SkColorGetB(background_color) / 255.0f,
                             SkColorGetA(background_color) / 255.0f);
  CGContextSetRGBFillColor(context, SkColorGetR(background_color) / 255.0f,
                           SkColorGetG(background_color) / 255.0f,
                           SkColorGetB(background_color) / 255.0f,
                           SkColorGetA(background_color) / 255.0f);
  CGContextFillRect(context, dirty.ToCGRect());
}

- (NSView*)hitTest:(NSPoint)point {
LOG(ERROR) << "$hitTest";
  if (![self shell])
    return [super hitTest:point];
  bool clickThrough = [self shell]->IsClickThrough();
  if (clickThrough) {
    return nil;  // i.e. this layer is not selectable
  } else {
    return [super hitTest:point];
  }
}

- (void) setBlockResizing: (BOOL)block {
  block_resizing_ = block;
}

- (void) setFrame: (NSRect)frameRect {
  if (block_resizing_ == NO) {
LOG(ERROR) << "ignoring setFrame(" << [self shell]->GetID() << "): {" << frameRect.origin.x << ", " << frameRect.origin.y << ", " << frameRect.size.width << ", " << frameRect.size.height << "}";
    [super setFrame:frameRect];
} else {
LOG(ERROR) << "setting setFrame(" << [self shell]->GetID() << "): {" << frameRect.origin.x << ", " << frameRect.origin.y << ", " << frameRect.size.width << ", " << frameRect.size.height << "}";
}
}

- (void) setFrameSize: (NSSize)newSize {
  if (block_resizing_ == NO) {
LOG(ERROR) << "ignoring setFrameSize(" << [self shell]->GetID() << "): {" << newSize.width << ", " << newSize.height << "}";
    [super setFrameSize:newSize];
} else {
LOG(ERROR) << "setting setFrameSize(" << [self shell]->GetID() << "): {" << newSize.width << ", " << newSize.height << "}";
}
}

- (void) setBounds: (NSRect)aRect {
  if (block_resizing_ == NO) {
LOG(ERROR) << "ignoring setBounds(" << [self shell]->GetID() << "): {" << aRect.origin.x << ", " << aRect.origin.y << ", " << aRect.size.width << ", " << aRect.size.height << "}";
    [super setBounds:aRect];
} else {
LOG(ERROR) << "setting setBounds(" << [self shell]->GetID() << "): {" << aRect.origin.x << ", " << aRect.origin.y << ", " << aRect.size.width << ", " << aRect.size.height << "}";
}
}

- (void) setBoundsSize: (NSSize)newSize {
  if (block_resizing_ == NO) {
LOG(ERROR) << "ignoring setBoundsSize(" << [self shell]->GetID() << "): {" << newSize.width << ", " << newSize.height << "}";
    [super setBoundsSize:newSize];
} else {
LOG(ERROR) << "setting setBoundsSize(" << [self shell]->GetID() << "): {" << newSize.width << ", " << newSize.height << "}";
}
}

- (void) resizeSubviewsWithOldSize: (NSSize)oldSize {
LOG(ERROR) << "$resizeSubviewsWithOldSize";
[super resizeSubviewsWithOldSize:oldSize];
}

- (void) displayIfNeededInRectIgnoringOpacity: (NSRect)aRect {
LOG(ERROR) << "$displayIfNeededInRectIgnoringOpacity";
[super displayIfNeededInRectIgnoringOpacity:aRect];
}

- (void) displayRectIgnoringOpacity: (NSRect)aRect 
                          inContext: (NSGraphicsContext *)context {
LOG(ERROR) << "$displayRectIgnoringOpacity";
[super displayRectIgnoringOpacity:aRect inContext:context];
}

- (BOOL) performKeyEquivalent: (NSEvent*)theEvent {
LOG(ERROR) << "$performKeyEquivalent";
return [super performKeyEquivalent:theEvent];
}

- (BOOL) performMnemonic: (NSString *)aString {
LOG(ERROR) << "$performMnemonic";
return [super performMnemonic:aString];
}

- (void)adjustPageHeightNew:(CGFloat *)newBottom 
                        top:(CGFloat)oldTop 
                     bottom:(CGFloat)oldBottom 
                      limit:(CGFloat)bottomLimit {
LOG(ERROR) << "$adjustPageHeightNew";
[super adjustPageHeightNew:newBottom top:oldTop bottom:oldBottom limit:bottomLimit];
}

- (void)adjustPageWidthNew:(CGFloat *)newRight 
                      left:(CGFloat)oldLeft 
                     right:(CGFloat)oldRight 
                     limit:(CGFloat)rightLimit {
LOG(ERROR) << "$adjustPageWidthNew";
[super adjustPageWidthNew:newRight left:oldLeft right:oldRight limit:rightLimit];
}

- (void) encodeWithCoder: (NSCoder*)aCoder {
LOG(ERROR) << "$encodeWithCoder";
[super encodeWithCoder:aCoder];
}

- (id) initWithCoder: (NSCoder*)aDecoder {
LOG(ERROR) << "$initWithCoder";
return [super initWithCoder:aDecoder];
}

@end

@implementation ElectronNativeVibrantView

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [super dealloc];
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
}

- (BOOL)isFlipped {
  return YES;
}

- (NSView*)hitTest:(NSPoint)point {
  if (![self shell])
    return [super hitTest:point];
  bool clickThrough = [self shell]->IsClickThrough();
  if (clickThrough) {
    return nil;  // i.e. this layer is not selectable
  } else {
    return [super hitTest:point];
  }
}

@end

#define kRMBlurredViewDefaultTintColor \
  [NSColor colorWithCalibratedWhite:1.0 alpha:0.7]
#define kRMBlurredViewDefaultSaturationFactor 2.0
#define kRMBlurredViewDefaultBlurRadius 20.0

@implementation ElectronNativeBlurredView

@synthesize _tintColor;
@synthesize _saturationFactor;
@synthesize _blurRadius;

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [super dealloc];
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
}

- (BOOL)isFlipped {
  return YES;
}

- (NSView*)hitTest:(NSPoint)point {
  if (![self shell])
    return [super hitTest:point];
  bool clickThrough = [self shell]->IsClickThrough();
  if (clickThrough) {
    return nil;  // i.e. this layer is not selectable
  } else {
    return [super hitTest:point];
  }
}

- (id)initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    [self setUp];
  }
  return self;
}

- (id)initWithCoder:(NSCoder*)coder {
  self = [super initWithCoder:coder];
  if (self) {
    [self setUp];
  }
  return self;
}

- (void)setTintColor:(NSColor*)color {
  _tintColor = color;

  // Since we need a CGColor reference, store it for the drawing of the layer.
  if (_tintColor) {
    [self.layer setBackgroundColor:_tintColor.CGColor];
  }

  // Trigger a re-drawing of the layer
  [self.layer setNeedsDisplay];
}

- (void)setBlurRadius:(float)radius {
  // Setting the blur radius requires a resetting of the filters
  _blurRadius = radius;
  [self resetFilters];
}

- (float)blurRadius {
  return _blurRadius;
}

- (void)setSaturationFactor:(float)factor {
  // Setting the saturation factor also requires a resetting of the filters
  _saturationFactor = factor;
  [self resetFilters];
}

- (float)saturationFactor {
  return _saturationFactor;
}

- (void)setUp {
  // Instantiate a new CALayer and set it as the NSView's layer (layer-hosting)
  _hostedLayer = [CALayer layer];
  [self nativeViewPrivate]->wants_layer = true;
  [self nativeViewPrivate]->wants_layer_infected = true;
  [self setWantsLayer:YES];
  [self setLayer:_hostedLayer];

  // Set up the default parameters
  _blurRadius = kRMBlurredViewDefaultBlurRadius;
  _saturationFactor = kRMBlurredViewDefaultSaturationFactor;
  [self setTintColor:kRMBlurredViewDefaultTintColor];

  // It's important to set the layer to mask to its bounds, otherwise the whole
  // parent view might get blurred
  [self.layer setMasksToBounds:YES];

  // To apply CIFilters on OS X 10.9, we need to set the property accordingly:
  if ([self respondsToSelector:@selector(setLayerUsesCoreImageFilters:)]) {
    BOOL flag = YES;
    NSInvocation* inv = [NSInvocation
        invocationWithMethodSignature:[self methodSignatureForSelector:@selector
                                            (setLayerUsesCoreImageFilters:)]];
    [inv setSelector:@selector(setLayerUsesCoreImageFilters:)];
    [inv setArgument:&flag atIndex:2];
    [inv invokeWithTarget:self];
  }

  // Set the layer to redraw itself once it's size is changed
  [self.layer setNeedsDisplayOnBoundsChange:YES];

  // Initially create the filter instances
  [self resetFilters];
}

- (void)resetFilters {
  // To get a higher color saturation, we create a ColorControls filter
  _saturationFilter = [CIFilter filterWithName:@"CIColorControls"];
  [_saturationFilter setDefaults];
  [_saturationFilter setValue:[NSNumber numberWithFloat:_saturationFactor]
                       forKey:@"inputSaturation"];

  // Next, we create the blur filter
  _blurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
  [_blurFilter setDefaults];
  [_blurFilter setValue:[NSNumber numberWithFloat:_blurRadius]
                 forKey:@"inputRadius"];

  // Now we apply the two filters as the layer's background filters
  [self.layer setBackgroundFilters:@[ _saturationFilter, _blurFilter ]];

  // ... and trigger a refresh
  [self.layer setNeedsDisplay];
}

@end

@implementation ElectronScaleRotateFlipView

- (void)dealloc {
  if ([self shell])
    [self shell]->NotifyViewIsDeleting();
  [super dealloc];
}

- (electron::NativeViewPrivate*)nativeViewPrivate {
  return &private_;
}

- (void)setNativeBackgroundColor:(SkColor)color {
  background_color_ = absl::make_optional(color);
  [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
  if (!background_color_.has_value())
    return;

  SkColor background_color = background_color_.value();
  gfx::RectF dirty(dirtyRect);
  CGContextRef context = reinterpret_cast<CGContextRef>(
      [[NSGraphicsContext currentContext] graphicsPort]);
  CGContextSetRGBStrokeColor(context, SkColorGetR(background_color) / 255.0f,
                             SkColorGetG(background_color) / 255.0f,
                             SkColorGetB(background_color) / 255.0f,
                             SkColorGetA(background_color) / 255.0f);
  CGContextSetRGBFillColor(context, SkColorGetR(background_color) / 255.0f,
                           SkColorGetG(background_color) / 255.0f,
                           SkColorGetB(background_color) / 255.0f,
                           SkColorGetA(background_color) / 255.0f);
  CGContextFillRect(context, dirty.ToCGRect());
}

- (NSView*)hitTest:(NSPoint)point {
  if (![self shell])
    return [super hitTest:point];
  bool clickThrough = [self shell]->IsClickThrough();
  if (clickThrough) {
    return nil;  // i.e. this layer is not selectable
  } else {
    return [super hitTest:point];
  }
}

- (void)initView {
  _scale = 1.0;
}

- (void)updateDimensions {
    //		[self setPostsBoundsChangedNotifications:NO];
    //		[self setPostsFrameChangedNotifications:NO];
    NSRect frame = [self frame];
    float scale = _scale;
    //	[super setPostsFrameChangedNotifications:YES];	- already set; with NO
    //there are no scrollers
    //	[super setPostsBoundsChangedNotifications:NO];	- no influence
    frame.origin = NSZeroPoint;
    [super
        setFrameSize:NSMakeSize(scale * frame.size.width,
                                scale * frame.size.height)];  // apply scaling
    //@@if ([self isFlipped])
      //@@[super scaleUnitSquareToSize:NSMakeSize(1.0, -1.0)];  // undo flipping
    [super setBounds:frame];  // make the same as contentView
    //@@if ([self isFlipped])
      //@@[super scaleUnitSquareToSize:NSMakeSize(1.0, -1.0)];
}

// FIXME: resizing isn't working well
- (void)repairClipViewBounds:(NSValue*)object {
  NSScrollView* scrollView = [self enclosingScrollView];
  NSClipView* clipView = [scrollView contentView];
  /* try to repair based on scroller positions */
  CGFloat floatValue = [[scrollView horizontalScroller] floatValue];
  NSRect documentRect = [clipView documentRect];
  NSRect clipBounds = [clipView bounds];
  NSPoint p;
  p.x = floatValue * (NSWidth(documentRect) - NSWidth(clipBounds));
  floatValue = [[scrollView verticalScroller] floatValue];
  p.y = (1.0 - floatValue) * (NSHeight(documentRect) - NSHeight(clipBounds));
  [clipView scrollToPoint:p];  // scroll clipview
  [self setNeedsDisplay:YES];
}

- (void)resizeWithOldSuperviewSize:
    (NSSize)oldSize {  // resizing the NSClipView must adjust scaling
  NSPoint center = NSZeroPoint;
  [super resizeWithOldSuperviewSize:oldSize];  // default behaviour first
  [NSObject
      cancelPreviousPerformRequestsWithTarget:self
                                     selector:@selector(repairClipViewBounds:)
                                       object:nil];
  [self performSelector:@selector(repairClipViewBounds:)
             withObject:[NSValue valueWithPoint:center]
             afterDelay:0.0];
}

- (BOOL)isFlipped {  // tells the drawing system we are flipped
  return YES;
}

- (float)scale {
  return _scale;
}

- (void)setScale:(float)scale {
  _scale = MAX(MIN(scale, 1e3), 1e-3);  // limit
  [self updateDimensions];              // trigger update of bounds
}

/* menu actions */

- (IBAction)zoomUnity:
    (id)sender {  // zoom content view to 100% i.e. 1/72 inch scale
  [self setScale:1.0];
}

- (IBAction)zoomIn:(id)sender {
  [self setScale:sqrt(2.0) * [self scale]];
}

- (IBAction)zoomOut:(id)sender {
  [self setScale:sqrt(0.5) * [self scale]];
}

@end

namespace electron {

namespace {

// Returns whether the view belongs to a frameless window.
bool IsFramelessWindow(NSView* view) {
  if (![view window])
    return false;
  if (![[view window] respondsToSelector:@selector(shell)])
    return false;
  return ![static_cast<ElectronNSWindow*>([view window]) shell]->has_frame();
}

// Create a fake mouse event.
NSEvent* FakeEvent(NSView* view, NSEventType type) {
  int windowNumber = [[view window] windowNumber];
  NSTimeInterval eventTime = [[NSApp currentEvent] timestamp];
  return [NSEvent enterExitEventWithType:type
                                location:NSZeroPoint
                           modifierFlags:0
                               timestamp:eventTime
                            windowNumber:windowNumber
                                 context:nil
                             eventNumber:0
                          trackingNumber:0xBADFACE
                                userData:nil];
}

// Following methods are overrided in ElectronNativeViewProtocol.

bool NativeViewInjected(NSView* self, SEL _cmd) {
  return true;
}

api::BaseView* GetShell(NSView* self, SEL _cmd) {
  return [self nativeViewPrivate]->shell;
}

BOOL AcceptsFirstResponder(NSView* self, SEL _cmd) {
  return [self nativeViewPrivate]->focusable;
}

void EnableTracking(NSView* self, SEL _cmd) {
  NativeViewPrivate* priv = [self nativeViewPrivate];
  if (priv->tracking_area)
    return;
  NSTrackingAreaOptions trackingOptions =
      NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved |
      NSTrackingActiveAlways | NSTrackingInVisibleRect;
  priv->tracking_area.reset([[NSTrackingArea alloc] initWithRect:NSZeroRect
                                                         options:trackingOptions
                                                           owner:self
                                                        userInfo:nil]);
  [self addTrackingArea:priv->tracking_area.get()];
}

void DisableTracking(NSView* self, SEL _cmd) {
  NativeViewPrivate* priv = [self nativeViewPrivate];
  if (priv->tracking_area) {
    [self removeTrackingArea:priv->tracking_area.get()];
    priv->tracking_area.reset();
  }
}

// Fix mouseExited isn't called when mouse leaves trackingArea while scrolling:
// https://stackoverflow.com/questions/8979639
void UpdateTrackingAreas(NSView* self, SEL _cmd) {
  auto super_impl = reinterpret_cast<decltype(&UpdateTrackingAreas)>(
      [[self superclass] instanceMethodForSelector:_cmd]);
  super_impl(self, _cmd);

  if (![self window])
    return;

  [self disableTracking];

  NSPoint mouse = [[self window] mouseLocationOutsideOfEventStream];
  mouse = [self convertPoint:mouse fromView:nil];

  NativeViewPrivate* priv = [self nativeViewPrivate];
  if (NSPointInRect(mouse, [self bounds])) {  // mouse in view.
    if (!priv->hovered)
      [self mouseEntered:FakeEvent(self, NSEventTypeMouseEntered)];
  } else {  // mouse not in view.
    if (priv->hovered)
      [self mouseExited:FakeEvent(self, NSEventTypeMouseExited)];
  }

  [self enableTracking];
}

// Following methods are overrided in ElectronNativeViewProtocol to make sure
// that content view of frameless always takes the size of its parent view.

// This method is directly called by NSWindow during a window resize on OSX
// 10.10.0, beta 2. We must override it to prevent the content view from
// shrinking.
void SetFrameSize(NSView* self, SEL _cmd, NSSize size) {
  if (IsFramelessWindow(self) && [self nativeViewPrivate]->is_content_view &&
      [self superview])
    size = [[self superview] bounds].size;

  NSSize old_size = [self bounds].size;

  auto super_impl = reinterpret_cast<decltype(&SetFrameSize)>(
      [[self superclass] instanceMethodForSelector:_cmd]);
  super_impl(self, _cmd, size);

  if (size.width != old_size.width || size.height != old_size.height)
    [self shell]->NotifySizeChanged(gfx::Size(old_size), gfx::Size(size));
}

// The contentView gets moved around during certain full-screen operations.
// This is less than ideal, and should eventually be removed.
void ViewDidMoveToSuperview(NSView* self, SEL _cmd) {
  if (!IsFramelessWindow(self) || ![self nativeViewPrivate]->is_content_view) {
    auto super_impl = reinterpret_cast<decltype(&ViewDidMoveToSuperview)>(
        [[self superclass] instanceMethodForSelector:_cmd]);
    super_impl(self, _cmd);
    return;
  }

  [self setFrame:[[self superview] bounds]];
}

}  // namespace

NativeViewPrivate::NativeViewPrivate() {}

NativeViewPrivate::~NativeViewPrivate() {}

bool IsNativeView(id view) {
  return [view respondsToSelector:@selector(nativeViewPrivate)];
}

void InstallNativeViewMethods(Class cl) {
  if ([cl instancesRespondToSelector:@selector(nativeViewInjected)])
    return;
  class_addMethod(cl, @selector(nativeViewInjected), (IMP)NativeViewInjected,
                  "B@:");

  class_addMethod(cl, @selector(shell), (IMP)GetShell, "^v@:");
  class_addMethod(cl, @selector(acceptsFirstResponder),
                  (IMP)AcceptsFirstResponder, "B@:");
  class_addMethod(cl, @selector(enableTracking), (IMP)EnableTracking, "v@:");
  class_addMethod(cl, @selector(disableTracking), (IMP)DisableTracking, "v@:");
  class_addMethod(cl, @selector(updateTrackingAreas), (IMP)UpdateTrackingAreas,
                  "v@");
  class_addMethod(cl, @selector(setFrameSize:), (IMP)SetFrameSize,
                  "v@:{_NSSize=ff}");
  class_addMethod(cl, @selector(viewDidMoveToSuperview),
                  (IMP)ViewDidMoveToSuperview, "v@:");
}

}  // namespace electron
