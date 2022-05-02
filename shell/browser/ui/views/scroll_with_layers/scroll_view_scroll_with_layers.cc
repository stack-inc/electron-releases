// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/scroll_with_layers/scroll_view_scroll_with_layers.h"

#include "ui/aura/window.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/native_widget_types.h"

#include "electron/shell/browser/ui/views/scroll_with_layers/native_view_host_scroll_with_layers.h"

namespace electron {

ScrollViewScrollWithLayers::ScrollViewScrollWithLayers()
    : ScrollView(views::ScrollView::ScrollWithLayers::kEnabled) {
  on_contents_layer_scrolled_subscription_ =
      this->AddContentsLayerScrolledCallback(
          base::BindRepeating(&ScrollViewScrollWithLayers::ContentsLayerScrolled,
              base::Unretained(this)));
}

ScrollViewScrollWithLayers::~ScrollViewScrollWithLayers() = default;

void ScrollViewScrollWithLayers::ContentsLayerScrolled() {
  scroll_ended_timier_.Start(FROM_HERE, base::Seconds(2), this,
      &ScrollViewScrollWithLayers::ContentsScrollEnded);

  ContentsLayerScrolledImpl(this);
  is_scrolling = true;
}

void ScrollViewScrollWithLayers::ContentsScrollEnded() {
  ContentsScrollEndedImpl(this);
  is_scrolling = false;
}

void ScrollViewScrollWithLayers::ContentsLayerScrolledImpl(View* view) {
  if (view->GetProperty(kNativeViewHostScrollWithLayer)) {
    NativeViewHostScrollWithLayers* native_view_host =
        static_cast<NativeViewHostScrollWithLayers*>(view);

    if (!is_scrolling) {
      gfx::NativeView native_view = native_view_host->native_view();
      if (native_view) {
        ui::Layer* layer = native_view->layer();
        if (layer) {
          layer->AddCacheRenderSurfaceRequest();
        }
      }
    }

    native_view_host->SetForceVisibleLayout(true);
    native_view_host->Layout();
  }

  for (auto* i : view->children()) {
    ContentsLayerScrolledImpl(i);
  }
}

void ScrollViewScrollWithLayers::ContentsScrollEndedImpl(views::View* view) {
  if (view->GetProperty(kNativeViewHostScrollWithLayer)) {
    NativeViewHostScrollWithLayers* native_view_host =
        static_cast<NativeViewHostScrollWithLayers*>(view);
    // native_view_host->SetForceVisibleLayout(false);
    native_view_host->Layout();

    if (is_scrolling) {
      gfx::NativeView native_view = native_view_host->native_view();
      if (native_view) {
        ui::Layer* layer = native_view->layer();
        if (layer) {
          layer->RemoveCacheRenderSurfaceRequest();
        }
      }
    }
  }

  for (auto* i : view->children()) {
    ContentsScrollEndedImpl(i);
  }
}

}  // namespace electron
