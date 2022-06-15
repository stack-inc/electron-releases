#include "shell/browser/ui/view_utils.h"

#include "shell/browser/ui/native_view.h"
#include "ui/views/view.h"

namespace electron {

void SetBoundsForView(NATIVEVIEW view,
                      const gfx::Rect& bounds,
                      const BoundsAnimationOptions& options,
                      NativeView* native_view /*= nullptr*/) {
  if (!options.animation || !native_view || !view->parent()) {
    view->SetBoundsRect(bounds);
    return;
  }

  if (options.use_from_bounds)
    view->SetBoundsRect(options.from_bounds);

  auto* bounds_animator = native_view->GetOrCreateBoundsAnimator();
  CHECK(bounds_animator);

  bounds_animator->SetAnimationDuration(
      base::Milliseconds(static_cast<int>(options.duration * 1000)));

  if (!options.use_control_points) {
    gfx::Tween::Type type = gfx::Tween::LINEAR;
    if (options.timing_function == TimingFunction::kLinear)
      type = gfx::Tween::LINEAR;
    else if (options.timing_function == TimingFunction::kEaseIn)
      type = gfx::Tween::EASE_IN;
    else if (options.timing_function == TimingFunction::kEaseOut)
      type = gfx::Tween::EASE_OUT;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      type = gfx::Tween::EASE_IN_OUT;
    else if (options.timing_function == TimingFunction::kDefault)
      type = gfx::Tween::EASE_OUT;
    bounds_animator->set_tween_type(type);
  } else {
    bounds_animator->set_cubic_bezier(options.cx1, options.cy1, options.cx2,
                                      options.cy2);
  }

  bounds_animator->AnimateViewTo(view, bounds);
}

void ResetScalingForView(NATIVEVIEW view) {}

void SetScaleForView(NATIVEVIEW view, const ScaleAnimationOptions& options) {}

float GetScaleXForView(NATIVEVIEW view) {
  return 1.0;
}

float GetScaleYForView(NATIVEVIEW view) {
  return 1.0;
}

void SetOpacityForView(NATIVEVIEW view,
                       const double opacity,
                       const AnimationOptions& options) {}

double GetOpacityForView(NATIVEVIEW view) {
  return 1.0;
}

}  // namespace electron
