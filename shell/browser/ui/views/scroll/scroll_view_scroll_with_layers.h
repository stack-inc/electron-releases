// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_SCROLL_WITH_LAYERS_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_SCROLL_WITH_LAYERS_H_

#include "base/timer/timer.h"
#include "shell/browser/ui/views/scroll/scroll_view.h"

namespace electron {

class ScrollViewScrollWithLayers : public ScrollView {
 public:
  ScrollViewScrollWithLayers();
  ~ScrollViewScrollWithLayers() override;

  ScrollViewScrollWithLayers(const ScrollViewScrollWithLayers&) = delete;
  ScrollViewScrollWithLayers(ScrollViewScrollWithLayers&&) = delete;

  ScrollViewScrollWithLayers& operator=(const ScrollViewScrollWithLayers&) =
      delete;
  ScrollViewScrollWithLayers& operator=(ScrollViewScrollWithLayers&&) = delete;

 private:
  void ContentsLayerScrolled();
  void ContentsScrolled();
  void ContentsScrollEnded();

  void ContentsLayerScrolledImpl(views::View* view);
  void ContentsScrollEndedImpl(views::View* view);

  base::CallbackListSubscription on_contents_layer_scrolled_subscription_;
  base::CallbackListSubscription on_contents_scrolled_subscription_;
  base::RetainingOneShotTimer scroll_ended_timier_;
  bool is_scrolling = false;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_SCROLL_VIEW_SCROLL_WITH_LAYERS_H_
