#include "shell/browser/ui/native_web_browser_view.h"

#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/view.h"

namespace electron {

void NativeWebBrowserView::InitWebBrowserView() {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  views::View* view = nullptr;
  if (iwc_view) {
    view = iwc_view->GetView();
    // the View is created and managed by InspectableWebContents.
    set_delete_view(false);
  } else {
    view = new views::View();
  }

  view->set_owned_by_client();
  SetNativeView(view);
}

void NativeWebBrowserView::RenderViewReady() {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->GetView()->Layout();
}

void NativeWebBrowserView::UpdateClickThrough() {
  bool click_through = IsClickThrough();
  if (GetNative())
    GetNative()->SetCanProcessEventsWithinSubtree(!click_through);

  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->SetClickThrough(click_through);
}

void NativeWebBrowserView::SetRoundedCorners(
    const RoundedCornersOptions& options) {
  NativeView::SetRoundedCorners(options);
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


}  // namespace electron
