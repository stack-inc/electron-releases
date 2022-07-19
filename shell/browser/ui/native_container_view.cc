#include "shell/browser/ui/native_container_view.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace electron {

NativeContainerView::NativeContainerView(bool vibrant, bool blurred)
    : NativeView(vibrant, blurred) {
  InitContainerView();
}

NativeContainerView::~NativeContainerView() = default;

bool NativeContainerView::IsContainer() const {
  return true;
}

void NativeContainerView::DetachChildView(NativeView* view) {
  if (RemoveChildView(view))
    NotifyChildViewDetached(view);
}

void NativeContainerView::SetWindowForChildren(NativeWindow* window) {
  for (auto view : children_)
    view->SetWindow(window);
}

void NativeContainerView::AddChildView(scoped_refptr<NativeView> view) {
  if (!GetNative() || !view)
    return;
  if (view == this || view->GetParent())
    return;

  view->SetParent(this);

  AddChildViewImpl(view.get());
  children_.insert(children_.begin() + ChildCount(), std::move(view));

  RearrangeChildViews();
}

bool NativeContainerView::RemoveChildView(NativeView* view) {
  if (!GetNative() || !view)
    return false;
  const auto i(std::find(children_.begin(), children_.end(), view));
  if (i == children_.end())
    return false;

  view->SetParent(nullptr);

  RemoveChildViewImpl(view);
  children_.erase(i);

  RearrangeChildViews();

  return true;
}

}  // namespace electron
