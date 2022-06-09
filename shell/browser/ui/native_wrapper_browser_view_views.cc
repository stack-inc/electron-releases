#include "shell/browser/ui/native_wrapper_browser_view.h"

#include "shell/browser/api/electron_api_browser_view.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/view.h"

namespace electron {

void NativeWrapperBrowserView::SetRoundedCorners(
    const RoundedCornersOptions& options) {
  NativeView::SetRoundedCorners(options);
  if (!api_browser_view_)
    return;
  InspectableWebContentsView* iwc_view =
      api_browser_view_->view()->GetInspectableWebContentsView();
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

void NativeWrapperBrowserView::InitWrapperBrowserView() {
  SetNativeView(new views::View());
}

void NativeWrapperBrowserView::SetBrowserViewImpl() {
  if (!GetNative())
    return;
  if (api_browser_view_->view()->GetInspectableWebContentsView()) {
    GetNative()->AddChildView(
        api_browser_view_->view()->GetInspectableWebContentsView()->GetView());
  }
}

void NativeWrapperBrowserView::DetachBrowserViewImpl() {
  if (!GetNative())
    return;
  if (api_browser_view_->view()->GetInspectableWebContentsView()) {
    GetNative()->RemoveChildView(
        api_browser_view_->view()->GetInspectableWebContentsView()->GetView());
  }
}

}  // namespace electron
