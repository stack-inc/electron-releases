// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/stack_elastic_scroll/stack_scroll_bar_views.h"

namespace electron {

StackScrollBarViews::StackScrollBarViews(bool horizontal)
    : views::ScrollBarViews(horizontal) {}

StackScrollBarViews::~StackScrollBarViews() = default;

bool StackScrollBarViews::OnMouseWheel(const ui::MouseWheelEvent& event) {
  if (!animator_)
    animator_ = std::make_unique<StackElasticScrollAnimator>(this);

  animator_->Start(event.x_offset(), event.y_offset());
  return true;
}

void StackScrollBarViews::OnGestureEvent(ui::GestureEvent* event) {
  if (animator_)
    animator_->Stop();

  views::ScrollBarViews::OnGestureEvent(event);
}

}
