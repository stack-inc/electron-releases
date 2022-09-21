// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_H_

#include "ui/views/controls/scroll_view.h"

namespace electron {

class ScrollView : public views::ScrollView {
 public:
  ScrollView();
  explicit ScrollView(views::ScrollView::ScrollWithLayers scroll_with_layers);
  ~ScrollView() override;

  ScrollView(const ScrollView&) = delete;
  ScrollView(ScrollView&&) = delete;

  ScrollView& operator=(const ScrollView&) = delete;
  ScrollView& operator=(ScrollView&&) = delete;

  // View overrides:
  void Layout() override;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_H_
