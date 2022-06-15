#include "shell/browser/ui/view_utils.h"

#include "shell/browser/ui/native_view.h"
#include "ui/compositor/layer.h"
#include "ui/views/view.h"

namespace electron {

void SetBoundsForView(NATIVEVIEW view,
                      const gfx::Rect& bounds,
                      const BoundsAnimationOptions& options,
                      NativeView* native_view /*= nullptr*/) {
  if (!options.animation || !native_view || !native_view->GetParent()) {
    view->SetBoundsRect(bounds);
    return;
  }

  if (options.use_from_bounds)
    view->SetBoundsRect(options.from_bounds);

  view->SetPaintToLayer();
  view->layer()->SetFillsBoundsOpaquely(false);
  view->layer()->SetFillsBoundsCompletely(false);

  auto* bounds_animator = native_view->GetParent()->GetOrCreateBoundsAnimator();
  bounds_animator->SetAnimationDuration(
      base::Milliseconds(static_cast<int>(options.duration * 1000)));

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
