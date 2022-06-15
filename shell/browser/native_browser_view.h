// Copyright (c) 2017 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_NATIVE_BROWSER_VIEW_H_
#define ELECTRON_SHELL_BROWSER_NATIVE_BROWSER_VIEW_H_

#include <vector>

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "shell/browser/ui/view_utils.h"
#include "shell/common/api/api.mojom.h"
#include "third_party/skia/include/core/SkColor.h"

namespace gfx {
class Image;
class Rect;
}  // namespace gfx

namespace electron {

enum AutoResizeFlags {
  kAutoResizeWidth = 0x1,
  kAutoResizeHeight = 0x2,
  kAutoResizeHorizontal = 0x4,
  kAutoResizeVertical = 0x8,
};

class InspectableWebContents;
class InspectableWebContentsView;

class NativeBrowserView : public content::WebContentsObserver {
 public:
  ~NativeBrowserView() override;

  // disable copy
  NativeBrowserView(const NativeBrowserView&) = delete;
  NativeBrowserView& operator=(const NativeBrowserView&) = delete;

  static NativeBrowserView* Create(
      InspectableWebContents* inspectable_web_contents);

  InspectableWebContents* GetInspectableWebContents() {
    return inspectable_web_contents_;
  }

  const std::vector<mojom::DraggableRegionPtr>& GetDraggableRegions() const {
    return draggable_regions_;
  }

  InspectableWebContentsView* GetInspectableWebContentsView();

  virtual void SetAutoResizeFlags(uint8_t flags) = 0;
  virtual void SetBounds(const gfx::Rect& bounds) = 0;
  virtual void SetBounds(const gfx::Rect& bounds,
                         const BoundsAnimationOptions& options) = 0;
  virtual gfx::Rect GetBounds() = 0;
  virtual void SetBackgroundColor(SkColor color) = 0;
  virtual void SetViewBounds(const gfx::Rect& bounds) = 0;
  virtual gfx::Rect GetViewBounds() = 0;
  virtual void ResetScaling() = 0;
  virtual void SetScale(const ScaleAnimationOptions& options) = 0;
  virtual float GetScaleX() = 0;
  virtual float GetScaleY() = 0;
  virtual void SetOpacity(const double opacity,
                          const AnimationOptions& options) = 0;
  virtual double GetOpacity() = 0;
  virtual void SetVisible(bool visible) = 0;
  virtual bool IsVisible() = 0;
  void ShowThumbnail(gfx::Image thumbnail);
  void HideThumbnail();

  virtual void UpdateDraggableRegions(
      const std::vector<gfx::Rect>& drag_exclude_rects) {}

  // Called when the window needs to update its draggable region.
  virtual void UpdateDraggableRegions(
      const std::vector<mojom::DraggableRegionPtr>& regions) {}

 protected:
  explicit NativeBrowserView(InspectableWebContents* inspectable_web_contents);
  // content::WebContentsObserver:
  void WebContentsDestroyed() override;

  InspectableWebContents* inspectable_web_contents_;
  std::vector<mojom::DraggableRegionPtr> draggable_regions_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_NATIVE_BROWSER_VIEW_H_
