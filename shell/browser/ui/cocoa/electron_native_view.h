#ifndef SHELL_BROWSER_UI_COCOA_ELECTRON_NATIVE_VIEW_H_
#define SHELL_BROWSER_UI_COCOA_ELECTRON_NATIVE_VIEW_H_

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include "base/mac/scoped_nsobject.h"
#include "shell/browser/ui/cocoa/mouse_capture.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "third_party/skia/include/core/SkColor.h"

@class NSTrackingArea;

namespace electron {

class NativeView;

// A private class that holds views specific private data.
// Object-C does not support multi-inheiritance, so it is impossible to add
// common data members for UI elements.
struct NativeViewPrivate {
  NativeViewPrivate();
  ~NativeViewPrivate();

  NativeView* shell = nullptr;
  bool focusable = true;
  bool hovered = false;
  bool is_content_view = false;
  bool wants_layer = false;
  bool wants_layer_infected = false;
  base::scoped_nsobject<NSTrackingArea> tracking_area;
  std::unique_ptr<MouseCapture> mouse_capture;
};

}  // namespace electron

// The methods that every View should implemented.
@protocol ElectronNativeViewProtocol
- (electron::NativeViewPrivate*)nativeViewPrivate;
- (void)setNativeBackgroundColor:(SkColor)color;
@end

@interface ElectronNativeView : NSView <ElectronNativeViewProtocol> {
 @private
  electron::NativeViewPrivate private_;
  absl::optional<SkColor> background_color_;
}
@end

@interface ElectronNativeVibrantView
    : NSVisualEffectView <ElectronNativeViewProtocol> {
 @private
  electron::NativeViewPrivate private_;
}
@end

@interface ElectronNativeBlurredView : NSView <ElectronNativeViewProtocol> {
 @private
  electron::NativeViewPrivate private_;
  // Keep a reference to the filters for later modification
  CIFilter *_blurFilter, *_saturationFilter;
  CALayer* _hostedLayer;
}

- (void)setTintColor:(NSColor*)color;
- (void)setBlurRadius:(float)radius;
- (float)blurRadius;
- (void)setSaturationFactor:(float)factor;
- (float)saturationFactor;

// The layer will be tinted using the tint color. By default it is a 70% White
// Color.
@property(strong, nonatomic) NSColor* _tintColor;

//* To get more vibrant colors, a filter to increase the saturation of the
//colors can be applied. The default value is 2.5.
@property(assign, nonatomic) float _saturationFactor;

//* The blur radius defines the strength of the Gaussian Blur filter. The
//default value is 20.0.
@property(assign, nonatomic) float _blurRadius;

@end

// Extended methods of ElectronNativeViewProtocol.
@interface NSView (ElectronNativeViewMethods) <ElectronNativeViewProtocol>
- (electron::NativeView*)shell;
- (void)enableTracking;
- (void)disableTracking;
@end

namespace electron {

// Return whether a class is part of views system.
bool IsNativeView(id view);

// Add custom view methods to class.
void InstallNativeViewMethods(Class cl);

}  // namespace electron

#endif  // SHELL_BROWSER_UI_COCOA_ELECTRON_NATIVE_VIEW_H_
