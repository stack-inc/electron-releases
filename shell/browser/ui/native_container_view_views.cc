#include "shell/browser/ui/native_container_view.h"

#include <utility>

#include "base/memory/ptr_util.h"
#include "ui/views/view.h"

namespace electron {

void NativeContainerView::InitContainerView() {
  SetNativeView(new views::View());
}

void NativeContainerView::AddChildViewImpl(NativeView* view) {
  if (!GetNative())
    return;
  view->set_delete_view(false);
  GetNative()->AddChildView(view->GetNative());
}

void NativeContainerView::RemoveChildViewImpl(NativeView* view) {
  if (!GetNative())
    return;
  view->set_delete_view(true);
  GetNative()->RemoveChildView(view->GetNative());
}

}  // namespace electron
