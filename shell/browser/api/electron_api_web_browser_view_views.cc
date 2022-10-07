// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_web_browser_view.h"

#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/view.h"

namespace electron {

namespace api {

void WebBrowserView::CreateWebBrowserView(
    InspectableWebContents* inspectable_web_contents) {
  InspectableWebContentsView* iwc_view =
      inspectable_web_contents ? inspectable_web_contents->GetView() : nullptr;
  views::View* view = nullptr;
  if (iwc_view) {
    view = iwc_view->GetView();
    // the View is created and managed by InspectableWebContents.
    set_delete_view(false);
  } else {
    view = new views::View();
  }

  SetView(view);
}

void WebBrowserView::RenderViewReady() {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->GetView()->Layout();
}

void WebBrowserView::SetRoundedCorners(const RoundedCornersOptions& options) {
  BaseView::SetRoundedCorners(options);

  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetView();
  view->SetPaintToLayer();
  view->layer()->SetFillsBoundsOpaquely(false);
  float radius = options.radius;
  auto corner_radii = gfx::RoundedCornersF(options.top_left ? radius : 0.0f,
                                           options.top_right ? radius : 0.0f,
                                           options.bottom_right ? radius : 0.0f,
                                           options.bottom_left ? radius : 0.0f);
  view->layer()->SetRoundedCornerRadius(corner_radii);
  view->layer()->SetIsFastRoundedCorner(true);
  iwc_view->SetCornerRadii(corner_radii);
}

void WebBrowserView::UpdateClickThrough() {
  bool click_through = IsClickThrough();
  if (GetView())
    GetView()->SetCanProcessEventsWithinSubtree(!click_through);

  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->SetClickThrough(click_through);
}

}  // namespace api

}  // namespace electron
