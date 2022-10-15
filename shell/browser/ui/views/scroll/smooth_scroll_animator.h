// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SMOOTH_SCROLL_ANIMATOR_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SMOOTH_SCROLL_ANIMATOR_H_

#include "base/memory/raw_ptr.h"
#include "ui/gfx/animation/animation_delegate.h"
#include "ui/views/animation/scroll_animator.h"
#include "ui/views/controls/scrollbar/scroll_bar.h"

namespace electron {

class SmoothScrollAnimator : public gfx::AnimationDelegate {
 public:
  SmoothScrollAnimator(views::ScrollBar* scroll_bar);
  ~SmoothScrollAnimator() override;

  SmoothScrollAnimator(const SmoothScrollAnimator&) = delete;
  SmoothScrollAnimator(SmoothScrollAnimator&&) = delete;
  SmoothScrollAnimator& operator=(const SmoothScrollAnimator&) = delete;
  SmoothScrollAnimator& operator=(SmoothScrollAnimator&&) = delete;

  void Start(int offset_x, int offset_y);
  void Stop();

 private:
  class ContinuousAnimation;

  // Implementation of gfx::AnimationDelegate.
  void AnimationProgressed(const gfx::Animation* animation) override;

  raw_ptr<views::ScrollBar> scroll_bar_;
  ContinuousAnimation* animation_;
  double dest_offset_x_;
  double dest_offset_y_;
  double current_offset_x_;
  double current_offset_y_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SMOOTH_SCROLL_ANIMATOR_H_
