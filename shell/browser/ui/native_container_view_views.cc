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

void NativeContainerView::RearrangeChildViews() {
  if (children_.size() == 0)
    return;

  std::list<NativeView*> children = {};
  for (auto it = children_.begin(); it != children_.end(); it++)
    children.push_back((*it).get());
  children.sort(
      [](auto* a, auto* b) { return a->GetZIndex() < b->GetZIndex(); });

  auto begin = children.begin();
  auto* first = *begin;
  begin++;

  for (auto it = begin; it != children.end(); it++) {
    auto* second = *it;

    int index_of_first = GetNative()->GetIndexOf(first->GetNative());
    int index_of_second = GetNative()->GetIndexOf(second->GetNative());
    if (index_of_second != index_of_first + 1)
      GetNative()->ReorderChildView(second->GetNative(), index_of_first + 1);

    first = second;
  }
}

void NativeContainerView::UpdateClickThrough() {
  NativeView::UpdateClickThrough();

  for (auto it = children_.begin(); it != children_.end(); it++)
    (*it)->UpdateClickThrough();
}

}  // namespace electron
