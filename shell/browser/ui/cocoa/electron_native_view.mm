#include "shell/browser/ui/cocoa/electron_native_view.h"

#include <objc/objc-runtime.h>

#include "base/mac/mac_util.h"
#include "base/mac/scoped_cftyperef.h"
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

NativeView* GetShell(NSView* self, SEL _cmd) {
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
      [self mouseEntered:FakeEvent(self, NSMouseEntered)];
  } else {  // mouse not in view.
    if (priv->hovered)
      [self mouseExited:FakeEvent(self, NSMouseExited)];
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
