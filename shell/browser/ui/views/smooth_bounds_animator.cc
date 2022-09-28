// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/smooth_bounds_animator.h"

namespace electron {

SmoothBoundsAnimator::SmoothBoundsAnimator(
    views::View* view, bool use_transforms)
        : views::BoundsAnimator(view, use_transforms) {
}

SmoothBoundsAnimator::~SmoothBoundsAnimator() = default;

void SmoothBoundsAnimator::AnimationContainerWasSet(
    gfx::AnimationContainer* container) {
  AnimationDelegateViews::AnimationContainerWasSet(container);

  if (!container)
    return;

  container->SetAnimationRunner(
      gfx::AnimationRunner::CreateDefaultAnimationRunner());
}

}  // namespace electron
