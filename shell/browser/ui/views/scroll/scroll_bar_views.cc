// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.


#include "build/build_config.h"
#if BUILDFLAG(IS_WIN)
#include <windows.h>
#endif

#include "shell/browser/ui/views/scroll/scroll_bar_views.h"

namespace electron {

ScrollBarViews::ScrollBarViews(bool horizontal)
    : views::ScrollBarViews(horizontal) {}

ScrollBarViews::~ScrollBarViews() = default;

bool ScrollBarViews::OnMouseWheel(const ui::MouseWheelEvent& event) {
  if (!animator_)
    animator_ = std::make_unique<SmoothScrollAnimator>(this);

  int x_offset = event.x_offset();
#if BUILDFLAG(IS_WIN)
  if (event.native_event().message == WM_MOUSEHWHEEL) {
    x_offset = -x_offset;
  }
#endif

  animator_->Start(x_offset, event.y_offset());
  return true;
}

void ScrollBarViews::OnGestureEvent(ui::GestureEvent* event) {
  if (animator_)
    animator_->Stop();

  views::ScrollBarViews::OnGestureEvent(event);
}

}  // namespace electron
