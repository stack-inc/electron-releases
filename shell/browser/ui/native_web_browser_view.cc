#include "shell/browser/ui/native_web_browser_view.h"

#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/gfx/image/image.h"

namespace electron {

NativeWebBrowserView::NativeWebBrowserView(
    InspectableWebContents* inspectable_web_contents)
    : inspectable_web_contents_(inspectable_web_contents) {
  InitWebBrowserView();
  Observe(inspectable_web_contents_->GetWebContents());
}

NativeWebBrowserView::~NativeWebBrowserView() = default;

void NativeWebBrowserView::AddObserver(Observer* observer) {
  CHECK(observer);
  observers_.AddObserver(observer);
}

void NativeWebBrowserView::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

InspectableWebContentsView*
NativeWebBrowserView::GetInspectableWebContentsView() {
  if (!inspectable_web_contents_)
    return nullptr;
  return inspectable_web_contents_->GetView();
}

void NativeWebBrowserView::ShowThumbnail(gfx::Image thumbnail) {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  iwc_view->ShowThumbnail(thumbnail);
}

void NativeWebBrowserView::HideThumbnail() {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  iwc_view->HideThumbnail();
}

void NativeWebBrowserView::WebContentsDestroyed() {
  inspectable_web_contents_ = nullptr;
}

void NativeWebBrowserView::SetWindowForChildren(NativeWindow* window) {
  NativeView::SetWindowForChildren(window);

  for (Observer& observer : observers_)
    observer.OnSettingOwnerWindow(window);
}

}  // namespace electron
