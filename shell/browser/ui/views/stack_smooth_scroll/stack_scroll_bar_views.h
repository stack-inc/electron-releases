// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_BAR_VIEWS_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_BAR_VIEWS_H_

#include "ui/views/controls/scrollbar/scroll_bar_views.h"

#include "electron/shell/browser/ui/views/stack_smooth_scroll/stack_smooth_scroll_animator.h"

namespace electron {

class StackScrollBarViews : public views::ScrollBarViews {
 public:
  explicit StackScrollBarViews(bool horizontal = true);
  ~StackScrollBarViews() override;

  StackScrollBarViews(const StackScrollBarViews&) = delete;
  StackScrollBarViews(StackScrollBarViews&&) = delete;

  StackScrollBarViews& operator=(const StackScrollBarViews&) = delete;
  StackScrollBarViews& operator=(StackScrollBarViews&&) = delete;

  // views::View
  bool OnMouseWheel(const ui::MouseWheelEvent& event) override;
  void OnGestureEvent(ui::GestureEvent* event) override;

 private:
  std::unique_ptr<StackSmoothScrollAnimator> animator_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_BAR_VIEWS_H_
