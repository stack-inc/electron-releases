// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2013 Adam Roben <adam@roben.org>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#ifndef ELECTRON_SHELL_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_VIEW_H_
#define ELECTRON_SHELL_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_VIEW_H_

#include <string>

#include "shell/common/api/api.mojom.h"
#include "ui/gfx/native_widget_types.h"

class DevToolsContentsResizingStrategy;

namespace gfx {
class Image;
class Point;
}  // namespace gfx

#if defined(TOOLKIT_VIEWS)
namespace gfx {
class RoundedCornersF;
}

namespace views {
class View;
}
#endif

namespace electron {

class InspectableWebContents;
class InspectableWebContentsViewDelegate;

class InspectableWebContentsView {
 public:
  explicit InspectableWebContentsView(
      InspectableWebContents* inspectable_web_contents);
  virtual ~InspectableWebContentsView();

  InspectableWebContents* inspectable_web_contents() {
    return inspectable_web_contents_;
  }

  // The delegate manages its own life.
  void SetDelegate(InspectableWebContentsViewDelegate* delegate) {
    delegate_ = delegate;
  }
  InspectableWebContentsViewDelegate* GetDelegate() const { return delegate_; }

  const std::vector<mojom::DraggableRegionPtr>& GetDraggableRegions() const {
    return draggable_regions_;
  }

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  // Returns the container control, which has devtools view attached.
  virtual views::View* GetView() = 0;

  // Returns the web view control, which can be used by the
  // GetInitiallyFocusedView() to set initial focus to web view.
  virtual views::View* GetWebView() = 0;
  virtual void SetCornerRadii(const gfx::RoundedCornersF& corner_radii) = 0;
  virtual void SetClickThrough(bool click_through) = 0;
#else
  virtual gfx::NativeView GetNativeView() const = 0;
#endif

  virtual void ShowDevTools(bool activate) = 0;
  // Hide the DevTools view.
  virtual void CloseDevTools() = 0;
  virtual bool IsDevToolsViewShowing() = 0;
  virtual bool IsDevToolsViewFocused() = 0;
  virtual void SetIsDocked(bool docked, bool activate) = 0;
  virtual void SetContentsResizingStrategy(
      const DevToolsContentsResizingStrategy& strategy) = 0;
  virtual void SetTitle(const std::u16string& title) = 0;
  virtual void ShowThumbnail(gfx::Image thumbnail) = 0;
  virtual void HideThumbnail() = 0;

  // Called when the window needs to update its draggable region.
  virtual void UpdateDraggableRegions(
      const std::vector<mojom::DraggableRegionPtr>& regions) = 0;

  virtual gfx::Point GetMouseLocation() = 0;

 protected:
  // Owns us.
  InspectableWebContents* inspectable_web_contents_;

  std::vector<mojom::DraggableRegionPtr> draggable_regions_;

 private:
  InspectableWebContentsViewDelegate* delegate_ = nullptr;  // weak references.
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_VIEW_H_
