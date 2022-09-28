// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SMOOTH_BOUNDS_ANIMATOR_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SMOOTH_BOUNDS_ANIMATOR_H_

#include "ui/views/animation/bounds_animator.h"

namespace electron {

class SmoothBoundsAnimator : public views::BoundsAnimator {
 public:
  SmoothBoundsAnimator(views::View* view, bool use_transforms = false);
  ~SmoothBoundsAnimator() override;

  SmoothBoundsAnimator(const SmoothBoundsAnimator&) = delete;
  SmoothBoundsAnimator(SmoothBoundsAnimator&&) = delete;
  SmoothBoundsAnimator& operator=(
      const SmoothBoundsAnimator&) = delete;
  SmoothBoundsAnimator& operator=(SmoothBoundsAnimator&&) = delete;

  // views::AnimationDelegateViews:
  void AnimationContainerWasSet(gfx::AnimationContainer* container) override;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SMOOTH_SCROLL_ANIMATOR_H_
