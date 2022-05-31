#include "shell/browser/ui/native_container_view.h"

#include <objc/objc-runtime.h>

#include <list>

#include "shell/browser/ui/cocoa/electron_native_view.h"

namespace electron {

void NativeContainerView::InitContainerView() {
  if (!IsVibrant())
    SetNativeView([[ElectronNativeView alloc] init]);
  else
    SetNativeView([[ElectronNativeVibrantView alloc] init]);
}

void NativeContainerView::AddChildViewImpl(NativeView* view) {
  [GetNative() addSubview:view->GetNative()];
  NativeViewPrivate* priv = [GetNative() nativeViewPrivate];
  if (priv->wants_layer_infected) {
    [view->GetNative() setWantsLayer:YES];
  } else {
    if (IsNativeView(view->GetNative()) &&
        [view->GetNative() nativeViewPrivate]->wants_layer_infected) {
      priv->wants_layer_infected = true;
      SetWantsLayer(true);
      for (int i = 0; i < ChildCount(); ++i)
        [children_[i]->GetNative() setWantsLayer:YES];
    }
  }
}

void NativeContainerView::RemoveChildViewImpl(NativeView* view) {
  [view->GetNative() removeFromSuperview];
  NSView* native_view = view->GetNative();
  if (IsNativeView(native_view))
    [native_view setWantsLayer:[native_view nativeViewPrivate]->wants_layer];
  else
    [native_view setWantsLayer:NO];
}

void NativeContainerView::RearrangeChildViews() {
  if (children_.size() == 0)
    return;

  [CATransaction begin];
  [CATransaction setDisableActions:YES];

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

    auto* nativeFirst = first->GetNative();
    auto* nativeSecond = second->GetNative();

    [GetNative() addSubview:nativeSecond
                 positioned:NSWindowAbove
                 relativeTo:nativeFirst];

    first = second;
  }

  [CATransaction commit];
}

void NativeContainerView::UpdateDraggableRegions() {
  for (auto view : children_)
    view->UpdateDraggableRegions();
}

}  // namespace electron
