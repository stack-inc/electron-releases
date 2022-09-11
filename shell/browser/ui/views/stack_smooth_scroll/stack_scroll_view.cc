// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/stack_smooth_scroll/stack_scroll_view.h"

#include "base/trace_event/trace_event.h"

#include "ui/aura/window_occlusion_tracker.h"

namespace electron {

StackScrollView::StackScrollView()
    : ScrollView() {
}

StackScrollView::StackScrollView(
    views::ScrollView::ScrollWithLayers scroll_with_layers)
        : ScrollView(scroll_with_layers) {
}

StackScrollView::~StackScrollView() = default;

void StackScrollView::Layout() {
  TRACE_EVENT1("views", "StackScrollView::Layout", "class", "StackScrollView");
  aura::WindowOcclusionTracker::ScopedPause pause_occlusion;
  ScrollView::Layout();
}

bool StackScrollView::OnMouseWheel(const ui::MouseWheelEvent& e) {
  LOG(ERROR) << this << " : -------------------> StackScrollView::OnMouseWheel";
  return views::ScrollView::OnMouseWheel(e);
}

void StackScrollView::OnScrollEvent(ui::ScrollEvent* event) {
  LOG(ERROR) << this << " : ------------------> StackScrollView::OnScrollEvent";
  views::ScrollView::OnScrollEvent(event);
}

void StackScrollView::OnGestureEvent(ui::GestureEvent* event) {
  LOG(ERROR) << this << " : -----------------> StackScrollView::OnGestureEvent";
  return views::ScrollView::OnGestureEvent(event);
}

}  // namespace electron
