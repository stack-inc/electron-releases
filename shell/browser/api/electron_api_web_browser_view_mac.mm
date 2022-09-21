// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_web_browser_view.h"

#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/gfx/geometry/rect.h"

// Match view::Views behavior where the view sticks to the top-left origin.
const NSAutoresizingMaskOptions kDefaultAutoResizingMask =
    NSViewMaxXMargin | NSViewMinYMargin;

namespace electron {

namespace api {

void WebBrowserView::CreateWebBrowserView(
    InspectableWebContents* inspectable_web_contents) {
  SetNativeView([[ElectronNativeView alloc] init]);

  InspectableWebContentsView* iwc_view =
      inspectable_web_contents ? inspectable_web_contents->GetView() : nullptr;
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = kDefaultAutoResizingMask;
  [GetNSView() addSubview:view];
}

void WebBrowserView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  BaseView::SetBounds(bounds, args);

  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.frame = NSMakeRect(0, 0, bounds.width(), bounds.height());

  // Ensure draggable regions are properly updated to reflect new bounds.
  iwc_view->UpdateDraggableRegions(iwc_view->GetDraggableRegions());
}

}  // namespace api

}  // namespace electron
