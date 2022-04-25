// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_NATIVE_VIEW_HOST_SCROLL_WITH_LAYERS_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_NATIVE_VIEW_HOST_SCROLL_WITH_LAYERS_H_

#include "ui/views/controls/native/native_view_host.h"

namespace electron {

extern const ui::ClassProperty<bool>* const kNativeViewHostScrollWithLayer;

class NativeViewHostScrollWithLayers : public views::NativeViewHost {
 public:
  NativeViewHostScrollWithLayers();
  ~NativeViewHostScrollWithLayers() override;

  NativeViewHostScrollWithLayers(
      const NativeViewHostScrollWithLayers&) = delete;
  NativeViewHostScrollWithLayers(NativeViewHostScrollWithLayers&&) = delete;

  NativeViewHostScrollWithLayers& operator=(
      const NativeViewHostScrollWithLayers&) = delete;
  NativeViewHostScrollWithLayers& operator=(
      NativeViewHostScrollWithLayers&&) = delete;

  void Layout() override;

  void SetForceVisibleLayout(bool visible);

 protected:
  void OnVisibleBoundsChanged() override;

 private:
  bool first_layout_visible_ = true;
  bool force_visible_layout_ = false;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_NATIVE_VIEW_HOST_SCROLL_WITH_LAYERS_H_
