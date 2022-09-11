// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_VIEW_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_VIEW_H_

#include "ui/views/controls/scroll_view.h"

namespace electron {

class StackScrollView : public views::ScrollView {
 public:
  StackScrollView();
  explicit StackScrollView(
      views::ScrollView::ScrollWithLayers scroll_with_layers);
  ~StackScrollView() override;

  StackScrollView(const StackScrollView&) = delete;
  StackScrollView(StackScrollView&&) = delete;

  StackScrollView& operator=(const StackScrollView&) = delete;
  StackScrollView& operator=(StackScrollView&&) = delete;

  // View overrides:
  void Layout() override;
  bool OnMouseWheel(const ui::MouseWheelEvent& e) override;
  void OnScrollEvent(ui::ScrollEvent* event) override;
  void OnGestureEvent(ui::GestureEvent* event) override;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_STACK_SMOOTH_SCROLL_STACK_SCROLL_VIEW_H_
