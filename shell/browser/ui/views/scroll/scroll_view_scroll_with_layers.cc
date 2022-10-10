// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/ui/views/scroll/scroll_view_scroll_with_layers.h"

#include "shell/browser/ui/views/scroll/native_view_host_scroll_with_layers.h"
#include "ui/aura/window.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/native_widget_types.h"

namespace electron {

ScrollViewScrollWithLayers::ScrollViewScrollWithLayers()
    : ScrollView(views::ScrollView::ScrollWithLayers::kEnabled) {
  on_contents_layer_scrolled_subscription_ =
      this->AddContentsLayerScrolledCallback(base::BindRepeating(
          &ScrollViewScrollWithLayers::ContentsLayerScrolled,
          base::Unretained(this)));
  on_contents_scrolled_subscription_ =
      this->AddContentsScrolledCallback(base::BindRepeating(
          &ScrollViewScrollWithLayers::ContentsScrolled,
          base::Unretained(this)));
}

ScrollViewScrollWithLayers::~ScrollViewScrollWithLayers() = default;

void ScrollViewScrollWithLayers::ContentsLayerScrolled() {
  scroll_ended_timier_.Start(FROM_HERE, base::Seconds(2), this,
                             &ScrollViewScrollWithLayers::ContentsScrollEnded);

  ContentsLayerScrolledImpl(this);
  is_scrolling = true;
  Layout();
}

void ScrollViewScrollWithLayers::ContentsScrolled() {
  if(contents())
    contents()->SchedulePaintInRect(contents()->GetVisibleBounds());
}

void ScrollViewScrollWithLayers::ContentsScrollEnded() {
  ContentsScrollEndedImpl(this);
  is_scrolling = false;
  Layout();
  if(contents())
    contents()->SchedulePaintInRect(contents()->GetVisibleBounds());
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

    gfx::Rect vis_bounds = native_view_host->GetVisibleBounds();
    if (!vis_bounds.IsEmpty()) {
      native_view_host->SetForceVisibleLayout(true);
    }
  }

  for (auto* i : view->children()) {
    ContentsLayerScrolledImpl(i);
  }
}

void ScrollViewScrollWithLayers::ContentsScrollEndedImpl(views::View* view) {
  if (view->GetProperty(kNativeViewHostScrollWithLayer)) {
    NativeViewHostScrollWithLayers* native_view_host =
        static_cast<NativeViewHostScrollWithLayers*>(view);
    native_view_host->SetForceVisibleLayout(false);

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
