#include "shell/browser/ui/cocoa/electron_native_view.h"

#include <objc/objc-runtime.h>

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

- (id)initWithFrame:(NSRect)frame {
  if ((self = [super initWithFrame:frame])) {
    _scale = 1.0;  // initialize
                   //		_autoMagnifyOnResize=YES;
                   //		[[NSNotificationCenter defaultCenter] addObserver:self
    //selector:@selector(debug:) name:NSViewFrameDidChangeNotification
    //object:nil];
    //		[[NSNotificationCenter defaultCenter] addObserver:self
    //selector:@selector(debug:) name:NSViewBoundsDidChangeNotification
    //object:nil];
  }
  return self;
}

#if 0
- (void) debug:(NSNotification *) n
{
	NSLog(@"%@ bounds=%@", n, NSStringFromRect([[self superview] bounds]));
//	if([n object] == self)	// don't care if we are triggered by e.g. NSScroller
		if(!NSEqualRects([[self superview] bounds], _prev))
			{
			_prev=[[self superview] bounds];
			}
}
#endif

- (BOOL)wantsDefaultClipping {
  return NO;
}  // do not clip subview to our bounds

- (void)viewDidMoveToSuperview {  // set default scale - we now have a superview
                                  // and enclosingScrollView
  [self setScale:1.0];
}

/* basic principle:
 * we are the documentView of the NSClipView
 * our frame determines scroller dimensions (in relation to the NSClipView
 * frame) the clip view moves us around (and our contentView) hence we scale our
 * frame according to the scale factor based on our content view frame size
 * (ignoring what is set here!) we set the origin of the content view's frame to
 * NSZeroPoint so that our bounds match the content view if unrotated we keep
 * our own bounds the same size as our content view
 */

- (void)updateDimensions {
  NSView* contentView;
  if ((contentView = [self contentView])) {
    //		[self setPostsBoundsChangedNotifications:NO];
    //		[self setPostsFrameChangedNotifications:NO];
    NSPoint center = [self center];  // will be restored/kept stable
    NSRect frame = [contentView frame];
    float angle = _rotationAngle, scale = _scale, factor;
    //	[super setPostsFrameChangedNotifications:YES];	- already set; with NO
    //there are no scrollers
    //	[super setPostsBoundsChangedNotifications:NO];	- no influence
    frame.origin = NSZeroPoint;
    factor = M_SQRT2 - ((M_SQRT2 - 1.0) * cos(M_PI / 45.0 * _rotationAngle));
    factor = 1 + (M_SQRT2 / 2) * fabs(sin(M_PI / 45.0 * _rotationAngle));
    factor = 1;
    NSLog(@"factor=%lf", factor);
    scale *= factor;  // scale up if rotated
    // FIXME: we may have to move the origin
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [super
        setFrameSize:NSMakeSize(scale * frame.size.width,
                                scale * frame.size.height)];  // apply scaling
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [contentView setFrameOrigin:frame.origin];  // align with our (0, 0)
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    // we have to undo rotation and flipping before setBounds since the result
    // seems to depend on the current internal matrix
    [super setBoundsRotation:0];  // unrotate before setting bounds
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    if (_boundsAreFlipped)
      [super scaleUnitSquareToSize:NSMakeSize(1.0, -1.0)];  // undo flipping
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [super setBounds:frame];  // make the same as contentView
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [super translateOriginToPoint:NSMakePoint(
                                      NSMidX(frame),
                                      NSMidY(frame))];  // rotate around center
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    if ((_boundsAreFlipped = [self isFlipped])) {
      angle = -angle;
      [super scaleUnitSquareToSize:NSMakeSize(1.0, -1.0)];
    }
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    //		BOOL bn=[self postsBoundsChangedNotifications];
    //		BOOL fn=[self postsFrameChangedNotifications];
    [super setBoundsRotation:angle];
    [super setFrameRotation:0];  // this sometimes also resets
                                 // clipview.bounds.origin to 0 removing any
                                 // scrolling position
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    //		NSScrollView *scrollView=[self enclosingScrollView];
    //		NSClipView *clipView=[scrollView contentView];
    //		[clipView scrollToPoint:b.origin];	// restore
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [super translateOriginToPoint:NSMakePoint(
                                      -NSMidX(frame),
                                      -NSMidY(frame))];  // rotate around center
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    [self setCenter:center];  // do all scaling and rotation around saved center
                              // of NSClipView
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    //		[self setPostsFrameChangedNotifications:fn];	// this alone posts the
    //notification! 		[self setPostsBoundsChangedNotifications:bn]; 		NSLog(@"%@",
    //NSStringFromRect([[self superview] bounds]));
  }
}

#if 0
- (void) setFrameSize:(NSSize) frame
{ // here we apply scaling and rotation to contentView and self
	NSPoint center=[self center];
	[super setFrameSize:frame];
	[self setCenter:center];	// do all scaling and rotation around center of NSClipView
}
#endif

// FIXME: resizing isn't working well
- (void)repairClipViewBounds:(NSValue*)object {
  NSScrollView* scrollView = [self enclosingScrollView];
  NSClipView* clipView = [scrollView contentView];
  NSLog(@"restore to %@", object);
  //	[clipView scrollToPoint:[object rectValue].origin];	// restore
  //	[self setCenter:[object pointValue]];	// restore
  // does not take into account the current scroller position
  // we need some [clipView reflectScrollers:scrollView]
  // but that *is* [clipView scrollToPoint:pt]
  // where pt is calculated from the scroller position, the document
  // width/height and clip bounds width/height
  //	[self setCenter:_center];	// restore
  //	[self setNeedsDisplay:YES];
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
  NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
  NSScrollView* scrollView = [self enclosingScrollView];
  NSClipView* clipView = [scrollView contentView];
  NSRect newBounds = [clipView bounds];  // new superview size
  NSPoint center = [self center];
  NSSize newSize = newBounds.size;
  NSLog(@"hscroller=%lf vscroller=%lf",
        [[scrollView horizontalScroller] floatValue],
        [[scrollView verticalScroller] floatValue]);
  [super resizeWithOldSuperviewSize:oldSize];  // default behaviour first
  NSLog(@"%@", NSStringFromRect([clipView bounds]));
  // any change here does not keep center stable and makes the superview bounds
  // origin go to (0, 0) but only on the second call! BTW: it happens in
  // -updateDimension resp. -setFrameRotation:0 there and it happens only if we
  // are "zoomed out" or "zoom to fit" so that there are no scrollers! if there
  // are scrollers, everything is fine in detail: only the axis w/o scroller is
  // reset - as if there is a [scroller floatValue] with scroller == nil BEI
  // ECad passiert das aber auch bei ZoomFit aber nicht Zoom1:1 Fällt bei Demo
  // vielleicht nicht so auf weil contentFrame.origin == NSZerpoPoint Hinweis:
  // die Scroller haben immer noch Werte zwischen 0 und 1 - sind nur nicht
  // sichtbar d.h. das Problem besteht nur bei unsichtbaren scrollern
  if (_autoMagnifyOnResize) {  // keep contentView unscaled by resizing
    // this is also broken because oldSize and newSize are not consistent
    // this only reduces the image!?!
    float scalex = newSize.width / oldSize.width;
    float scaley = newSize.height / oldSize.height;
    NSLog(@"scalex=%lf scaley=%lf", scalex, scaley);
    // taking average scale factor is quite intuitive but not perfect...
    // note: it does not keep the center constant
    [self setScale:[self scale] * (0.5 * (scalex + scaley))];
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
  } else {
    //		NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
    /*
     * setFrameRotation:0 while resizing seems to reset [superview
     * bounds].origin to NSZeroPoint effectively removing any previous position
     * I could only find out that it is likely triggered by re-tiling the
     * NSScrollView but that happens far later than we know here Hence we need a
     * trick to do it after drawing
     */
    //		[self updateDimensions];
  }
  // a single coalescing notifier would suffice - but occurs only AFTER visual
  // tracking feedback runloop
  [NSObject
      cancelPreviousPerformRequestsWithTarget:self
                                     selector:@selector(repairClipViewBounds:)
                                       object:nil];
  [self performSelector:@selector(repairClipViewBounds:)
             withObject:[NSValue valueWithPoint:center]
             afterDelay:0.0];

  NSLog(@"%@", NSStringFromRect([[self superview] bounds]));
}

- (BOOL)isHorizontallyFlipped {
  return _isHorizontallyFlipped;
}

- (BOOL)isFlipped {  // tells the drawing system we are flipped
  return _isVerticallyFlipped;
}

- (void)setFlipped:(BOOL)flag {
  _isVerticallyFlipped = flag;
  [self updateDimensions];
}

- (NSView*)contentView {
  NSArray* a = [self subviews];
  return [a count] ? [a objectAtIndex:0] : nil;
}

- (void)setContentView:(NSView*)object {  // replace subview or add subview
  NSView* contentView = [self contentView];
  if (!contentView)
    [self addSubview:object];
  else if (contentView != object)
    [self replaceSubview:contentView with:object];
  [self updateDimensions];  // trigger update of bounds
}

- (NSRect)contentFrame {
  NSView* cv = [self contentView];
  if (!cv)
    return NSZeroRect;
  if ([cv respondsToSelector:@selector(activeFrame)])
    return [cv activeFrame];
  return [cv frame];
}

- (NSPoint)
    center {  // get center of currently visible area in document coordinates
  NSScrollView* scrollView = [self enclosingScrollView];
  if (scrollView) {
    NSClipView* clipView = [scrollView contentView];
    NSRect clipViewFrame = [clipView frame];
    NSPoint clipViewCenter =
        NSMakePoint(NSMidX(clipViewFrame), NSMidY(clipViewFrame));
    return [[self contentView] convertPoint:clipViewCenter fromView:scrollView];
  }
  return NSZeroPoint;
}

- (void)setCenter:
    (NSPoint)center {  // move this content point to center of view
  NSScrollView* scrollView;
  NSClipView* clipView;
  if ((scrollView = [self enclosingScrollView]) &&
      (clipView = [scrollView contentView])) {
    NSRect cvbounds = [clipView bounds];
    NSPoint cvcenter = [[self contentView] convertPoint:center toView:clipView];
    cvcenter.x -= 0.5 * NSWidth(cvbounds);
    cvcenter.y -= 0.5 * NSHeight(cvbounds);
    [clipView scrollToPoint:cvcenter];
    [scrollView reflectScrolledClipView:clipView];
    [self setNeedsDisplay:YES];
  }
  _center = center;
}

- (float)scale {
  return _scale;
}

- (void)setScaleValue:(float)scale {
  _scale = scale;
}

- (void)setScale:(float)scale {
  _scale = MAX(MIN(scale, 1e3), 1e-3);  // limit
  [self updateDimensions];              // trigger update of bounds
}

- (void)setScaleForRect:
    (NSRect)area {  // scale so that area fills the clip view - and center
  NSClipView* clipView;
  if (!NSIsEmptyRect(area) &&
      (clipView = [[self enclosingScrollView] contentView])) {
    NSRect frame = [clipView frame];  // NSClipView frame
    [self setScale:MIN(NSWidth(frame) / NSWidth(area),
                       NSHeight(frame) / NSHeight(area))];
    [self setCenter:NSMakePoint(NSMidX(area), NSMidY(area))];
  }
}

- (int)rotationAngle {
  return _rotationAngle;
}

- (void)setRotationAngle:(int)angle {
  _rotationAngle =
      ((angle % 360) + 360) % 360;  // also works for negative values
  [self updateDimensions];          // trigger update of bounds
}

/* menu actions */

- (IBAction)center:
    (id)sender {  // center the main view (independently of scaling)
  NSRect area = [self contentFrame];
  [self setCenter:NSMakePoint(NSMidX(area), NSMidY(area))];
}

- (IBAction)zoomFit:(id)sender {  // zoom content view to fit clip view
  NSRect area = [self contentFrame];
  [self setScaleForRect:area];  // this also centers the area
}

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

- (IBAction)rotateImageLeft:(id)sender {
  [self setRotationAngle:[self rotationAngle] + 10];
}

- (IBAction)rotateImageRight:(id)sender {
  [self setRotationAngle:[self rotationAngle] - 10];
}

- (IBAction)rotateImageLeft90:(id)sender {
  [self setRotationAngle:[self rotationAngle] + 90];
}

- (IBAction)rotateImageRight90:(id)sender {
  [self setRotationAngle:[self rotationAngle] - 90];
}

- (IBAction)rotateImageUpright:(id)sender {
  [self setRotationAngle:0];
}

- (IBAction)flipHorizontal:(id)sender {
  [self setRotationAngle:[self rotationAngle] - 180];
  _isHorizontallyFlipped = !_isHorizontallyFlipped;
  [self flipVertical:sender];
}

- (IBAction)flipVertical:(id)sender {
  [self setFlipped:![self isFlipped]];
}

- (IBAction)unflip:(id)sender {
  if (_isHorizontallyFlipped)
    [self flipHorizontal:sender];  // make it an even number of horizontal flips
  [self setFlipped:NO];
}

- (void)scrollWheel:(NSEvent*)event {
  if ([event modifierFlags] &
      NSEventModifierFlagOption) {  // translate scrollwheel up/down into zoom
    [self setScale:pow(1.2, [event scrollingDeltaY]) * [self scale]];
    // rotate left/right by [event scrollingDeltaX]?
  } else
    [super scrollWheel:event];  // handle by ClipView/ScrollView (for panning)
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
