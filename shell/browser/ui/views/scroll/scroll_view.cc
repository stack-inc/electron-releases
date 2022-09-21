// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/ui/views/scroll/scroll_view.h"

#include "base/trace_event/trace_event.h"

#include "ui/aura/window_occlusion_tracker.h"

namespace electron {

ScrollView::ScrollView() : views::ScrollView() {}

ScrollView::ScrollView(views::ScrollView::ScrollWithLayers scroll_with_layers)
    : views::ScrollView(scroll_with_layers) {}

ScrollView::~ScrollView() = default;

void ScrollView::Layout() {
  TRACE_EVENT1("views", "ScrollView::Layout", "class", "ScrollView");
  aura::WindowOcclusionTracker::ScopedPause pause_occlusion;
  views::ScrollView::Layout();
}

}  // namespace electron
