// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/ui/views/scroll/web_view_scroll_with_layers.h"

#include "shell/browser/ui/views/inspectable_web_contents_view_views.h"

namespace electron {

void WebViewScrollWithLayers::DidFirstVisuallyNonEmptyPaint() {
  if (!iwcvv_)
    return;

  iwcvv_->SetStopPaintBackground(true);
}

}  // namespace electron
