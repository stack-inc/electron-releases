#include "shell/browser/ui/native_view.h"

#include "shell/browser/native_window.h"

namespace electron {

#if defined(OS_MAC)
bool NativeView::Observer::OnMouseDown(NativeView* observed_view,
                                       const NativeMouseEvent& event) {
  return false;
}

bool NativeView::Observer::OnMouseUp(NativeView* observed_view,
                                     const NativeMouseEvent& event) {
  return false;
}
#endif  // defined(OS_MAC)

NativeView::NativeView() : view_(nullptr) {
  InitView();
}

NativeView::~NativeView() {
  DestroyView();
}

void NativeView::SetVisible(bool visible) {
  if (visible == IsVisible())
    return;
  SetVisibleImpl(visible);
}

void NativeView::SetParent(NativeView* parent) {
  if (parent) {
    SetWindow(parent->window_);
  } else {
    SetWindow(nullptr);
  }
  parent_ = parent;
}

void NativeView::BecomeContentView(NativeWindow* window) {
  SetWindow(window);
  parent_ = nullptr;
}

void NativeView::SetWindow(NativeWindow* window) {
  window_ = window;
  SetWindowForChildren(window);
}

void NativeView::SetWindowForChildren(NativeWindow* window) {}

bool NativeView::IsContainer() const {
  return false;
}

void NativeView::SetZIndex(int z_index) {
  z_index_ = z_index;
}

int NativeView::GetZIndex() const {
  return z_index_;
}

void NativeView::SetClickThrough(bool click_through) {
  is_click_through_ = click_through;
}

bool NativeView::IsClickThrough() const {
  return is_click_through_;
}

void NativeView::DetachChildView(NativeView* view) {}

void NativeView::TriggerBeforeunloadEvents() {}

void NativeView::AddObserver(Observer* observer) {
  CHECK(observer);
  observers_.AddObserver(observer);
}

void NativeView::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void NativeView::NotifyChildViewDetached(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnChildViewDetached(this, view);
}

#if defined(OS_MAC)
bool NativeView::NotifyMouseDown(const NativeMouseEvent& event) {
  bool prevent_default = false;
  for (Observer& observer : observers_)
    if (observer.OnMouseDown(this, event))
      prevent_default = true;
  return prevent_default;
}

bool NativeView::NotifyMouseUp(const NativeMouseEvent& event) {
  bool prevent_default = false;
  for (Observer& observer : observers_)
    if (observer.OnMouseUp(this, event))
      prevent_default = true;
  return prevent_default;
}

void NativeView::NotifyMouseMove(const NativeMouseEvent& event) {
  for (Observer& observer : observers_)
    observer.OnMouseMove(this, event);
}

void NativeView::NotifyMouseEnter(const NativeMouseEvent& event) {
  for (Observer& observer : observers_)
    observer.OnMouseEnter(this, event);
}

void NativeView::NotifyMouseLeave(const NativeMouseEvent& event) {
  for (Observer& observer : observers_)
    observer.OnMouseLeave(this, event);
}

void NativeView::NotifyCaptureLost() {
  for (Observer& observer : observers_)
    observer.OnCaptureLost(this);
}

void NativeView::NotifyDidScroll(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnDidScroll(view);
}

void NativeView::NotifyWillStartLiveScroll(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnWillStartLiveScroll(view);
}

void NativeView::NotifyDidLiveScroll(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnDidLiveScroll(view);
}

void NativeView::NotifyDidEndLiveScroll(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnDidEndLiveScroll(view);
}
#endif  // defined(OS_MAC)

void NativeView::NotifySizeChanged(gfx::Size old_size, gfx::Size new_size) {
  for (Observer& observer : observers_)
    observer.OnSizeChanged(this, old_size, new_size);
}

void NativeView::NotifyViewIsDeleting() {
  for (Observer& observer : observers_)
    observer.OnViewIsDeleting(this);
}

}  // namespace electron
