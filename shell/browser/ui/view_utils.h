#ifndef SHELL_BROWSER_UI_VIEW_UTILS_H_
#define SHELL_BROWSER_UI_VIEW_UTILS_H_

#include "build/build_config.h"
#include "gin/converter.h"
#include "ui/gfx/geometry/rect.h"

#if BUILDFLAG(IS_MAC)
#ifdef __OBJC__
@class NSView;
#else
struct NSView;
#endif
#elif defined(TOOLKIT_VIEWS)
namespace views {
class View;
}
#endif

namespace electron {

#if BUILDFLAG(IS_MAC)
using NATIVEVIEW = NSView*;
#elif defined(TOOLKIT_VIEWS)
using NATIVEVIEW = views::View*;
#endif

class NativeView;

enum class TimingFunction {
  kLinear,
  kEaseIn,
  kEaseOut,
  kEaseInEaseOut,
  kDefault,
};

struct AnimationOptions {
  AnimationOptions() = default;

  bool animation = false;
  float duration = 0.4;
  TimingFunction timing_function = TimingFunction::kLinear;
  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  bool use_control_points = false;
};

struct BoundsAnimationOptions : public AnimationOptions {
  BoundsAnimationOptions() = default;

  gfx::Rect from_bounds;
  bool use_from_bounds = false;
};

enum class AnchorXPos {
  kUnknown,
  kLeft,
  kCenter,
  kRight,
  kPercentage,
};

enum class AnchorYPos {
  kUnknown,
  kTop,
  kCenter,
  kBottom,
  kPercentage,
};

struct ScaleAnimationOptions : public AnimationOptions {
  ScaleAnimationOptions() = default;

  float scale_x = 1.0, scale_y = 1.0;
  bool adjust_frame = true;
  AnchorXPos anchor_x_pos = AnchorXPos::kUnknown;
  AnchorYPos anchor_y_pos = AnchorYPos::kUnknown;
  float anchor_x_percentage = 0.0, anchor_y_percentage = 0.0;
};

void SetBoundsForView(NATIVEVIEW view,
                      const gfx::Rect& bounds,
                      const BoundsAnimationOptions& options,
                      NativeView* native_view = nullptr);

// Makes the scaling of the receiver equal to the window's
// base coordinate system.
void ResetScalingForView(NATIVEVIEW view);
void SetScaleForView(NATIVEVIEW view, const ScaleAnimationOptions& options);
float GetScaleXForView(NATIVEVIEW view);
float GetScaleYForView(NATIVEVIEW view);

void SetOpacityForView(NATIVEVIEW view,
                       const double opacity,
                       const AnimationOptions& options);
double GetOpacityForView(NATIVEVIEW view);

}  // namespace electron

namespace gin {

template <>
struct Converter<electron::AnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::AnimationOptions* options);
};

template <>
struct Converter<electron::BoundsAnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::BoundsAnimationOptions* options);
};

template <>
struct Converter<electron::ScaleAnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::ScaleAnimationOptions* options);
};

}  // namespace gin

#endif  // SHELL_BROWSER_UI_VIEW_UTILS_H_
