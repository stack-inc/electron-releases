// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_BAR_VIEWS_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_BAR_VIEWS_H_

#include "shell/browser/ui/views/scroll/smooth_scroll_animator.h"
#include "ui/views/controls/scrollbar/scroll_bar_views.h"

namespace electron {

class ScrollBarViews : public views::ScrollBarViews {
 public:
  explicit ScrollBarViews(bool horizontal = true);
  ~ScrollBarViews() override;

  ScrollBarViews(const ScrollBarViews&) = delete;
  ScrollBarViews(ScrollBarViews&&) = delete;

  ScrollBarViews& operator=(const ScrollBarViews&) = delete;
  ScrollBarViews& operator=(ScrollBarViews&&) = delete;

  // views::View
  bool OnMouseWheel(const ui::MouseWheelEvent& event) override;
  void OnGestureEvent(ui::GestureEvent* event) override;

 private:
  std::unique_ptr<SmoothScrollAnimator> animator_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_BAR_VIEWS_H_
