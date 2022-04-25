// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/ui/views/native_view_host_scroll_with_layers.h"

#include "ui/views/controls/native/native_view_host_wrapper.h"

namespace electron {

DEFINE_UI_CLASS_PROPERTY_KEY(bool, kNativeViewHostScrollWithLayer, false)

NativeViewHostScrollWithLayers::NativeViewHostScrollWithLayers() {
  SetProperty(kNativeViewHostScrollWithLayer, true);
}

NativeViewHostScrollWithLayers::~NativeViewHostScrollWithLayers() = default;

void NativeViewHostScrollWithLayers::Layout() {
  if (!native_view() || !native_wrapper())
    return;

  gfx::Rect vis_bounds = GetVisibleBounds();
  bool visible = !vis_bounds.IsEmpty() || first_layout_visible_ ||
      force_visible_layout_;
  // TODO: Add option to not hide web contents when scrolling.
  // TODO: Cache wbe contents and view port when scrolling.
  if (!vis_bounds.IsEmpty())
    first_layout_visible_ = false;

  if (visible && !fast_resize()) {
    if (vis_bounds.size() != size()) {
      // Only a portion of the Widget is really visible.
      int x = vis_bounds.x();
      int y = vis_bounds.y();
      native_wrapper()->InstallClip(x, y, vis_bounds.width(),
                                  vis_bounds.height());
    } else if (native_wrapper()->HasInstalledClip()) {
      // The whole widget is visible but we installed a clip on the widget,
      // uninstall it.
      native_wrapper()->UninstallClip();
    }
  }

  if (visible) {
    // Since widgets know nothing about the View hierarchy (they are direct
    // children of the Widget that hosts our View hierarchy) they need to be
    // positioned in the coordinate system of the Widget, not the current
    // view.  Also, they should be positioned respecting the border insets
    // of the native view.
    gfx::Rect local_bounds = ConvertRectToWidget(GetContentsBounds());
    gfx::Size native_size =
        native_view_size().IsEmpty() ? local_bounds.size() : native_view_size();
    native_wrapper()->ShowWidget(local_bounds.x(), local_bounds.y(),
                                local_bounds.width(), local_bounds.height(),
                                native_size.width(),
                                native_size.height());
  } else {
    native_wrapper()->HideWidget();
  }
}

void NativeViewHostScrollWithLayers::SetForceVisibleLayout(bool visible) {
  force_visible_layout_ = visible;
}

void NativeViewHostScrollWithLayers::OnVisibleBoundsChanged() {
  Layout();
}

}  // namespace electron
