#include "shell/browser/ui/native_web_browser_view.h"

#include "shell/browser/ui/inspectable_web_contents_view.h"
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
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  iwc_view->SetClickThrough(IsClickThrough());
}

}  // namespace electron
