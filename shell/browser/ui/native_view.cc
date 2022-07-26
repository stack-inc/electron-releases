#include "shell/browser/ui/native_view.h"

#include "shell/browser/native_window.h"

namespace electron {

#if BUILDFLAG(IS_MAC)
bool NativeView::Observer::OnMouseDown(NativeView* observed_view,
                                       const NativeMouseEvent& event) {
  return false;
}

bool NativeView::Observer::OnMouseUp(NativeView* observed_view,
                                     const NativeMouseEvent& event) {
  return false;
}
#endif  // BUILDFLAG(IS_MAC)

NativeView::RoundedCornersOptions::RoundedCornersOptions() = default;

NativeView::ClippingInsetOptions::ClippingInsetOptions() = default;

NativeView::NativeView(bool vibrant, bool blurred)
    : view_(nullptr), vibrant_(vibrant), blurred_(blurred) {
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

NativeView::RoundedCornersOptions NativeView::GetRoundedCorners() const {
  return rounded_corners_;
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

void NativeView::DetachChildView(NativeView* view) {}

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

#if BUILDFLAG(IS_MAC)
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

void NativeView::NotifyScrollWheel(NativeView* view,
                                   bool mouse_event,
                                   float scrolling_delta_x,
                                   float scrolling_delta_y,
                                   std::string phase,
                                   std::string momentum_phase) {
  for (Observer& observer : observers_)
    observer.OnScrollWheel(view, mouse_event, scrolling_delta_x,
                           scrolling_delta_y, phase, momentum_phase);
}
#endif  // BUILDFLAG(IS_MAC)

void NativeView::NotifyDidScroll(NativeView* view) {
  for (Observer& observer : observers_)
    observer.OnDidScroll(view);
}

void NativeView::NotifySizeChanged(gfx::Size old_size, gfx::Size new_size) {
  for (Observer& observer : observers_)
    observer.OnSizeChanged(this, old_size, new_size);
}

void NativeView::NotifyViewIsDeleting() {
  for (Observer& observer : observers_)
    observer.OnViewIsDeleting(this);
}

}  // namespace electron
