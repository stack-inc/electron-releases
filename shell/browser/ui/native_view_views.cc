#include "shell/browser/ui/native_view.h"

#include <utility>

#include "base/memory/ptr_util.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/background.h"
#include "ui/views/view.h"
#include "ui/views/view_class_properties.h"

namespace electron {

void NativeView::SetNativeView(NATIVEVIEW view) {
  if (view_) {
    view_->RemoveObserver(this);
    if (delete_view_)
      delete view_;
  }

  view_ = view;
  view_->AddObserver(this);
}

void NativeView::InitView() {
  auto* view = new views::View();
  view->set_owned_by_client();
  SetNativeView(view);
}

void NativeView::DestroyView() {
  if (!view_)
    return;
  view_->RemoveObserver(this);
  if (delete_view_)
    delete view_;
}

void NativeView::OnViewBoundsChanged(views::View* observed_view) {
  if (!view_)
    return;
  gfx::Rect bounds = view_->bounds();
  gfx::Size size = bounds.size();
  gfx::Size old_size = bounds_.size();
  bounds_ = bounds;
  if (size != old_size)
    NotifySizeChanged(old_size, size);
}

void NativeView::OnViewRemovedFromWidget(views::View* observed_view) {
  if (bounds_animator_.get()) {
    bounds_animator_->Cancel();
    bounds_animator_.reset();
  }
}

void NativeView::OnViewIsDeleting(views::View* observed_view) {
  view_ = nullptr;
  NotifyViewIsDeleting();
}

void NativeView::OnViewHierarchyChanged(
    views::View* observed_view,
    const views::ViewHierarchyChangedDetails& details) {
  SetRoundedCorners(GetRoundedCorners());
  UpdateClickThrough();
  UpdateBlockScrollViewWhenFocus(IsBlockScrollViewWhenFocus());
}

void NativeView::SetBounds(const gfx::Rect& bounds,
                           const BoundsAnimationOptions& options) {
  if (view_)
    SetBoundsForView(view_, bounds, options, this);
}

gfx::Rect NativeView::GetBounds() const {
  if (view_)
    return view_->bounds();
  return gfx::Rect();
}

gfx::Point NativeView::OffsetFromView(const NativeView* from) const {
  if (!view_)
    return gfx::Point();
  gfx::Point point;
  views::View::ConvertPointToTarget(from->GetNative(), view_, &point);
  return point;
}

gfx::Point NativeView::OffsetFromWindow() const {
  if (!view_)
    return gfx::Point();
  gfx::Point point;
  views::View::ConvertPointFromWidget(view_, &point);
  return point;
}

void NativeView::SetVisibleImpl(bool visible) {
  if (view_)
    view_->SetVisible(visible);
}

bool NativeView::IsVisible() const {
  if (view_)
    return view_->GetVisible();
  return false;
}

bool NativeView::IsTreeVisible() const {
  return IsVisible();
}

void NativeView::Focus() {
  if (view_)
    view_->RequestFocus();
}

bool NativeView::HasFocus() const {
  if (view_)
    return view_->HasFocus();
  return false;
}

void NativeView::SetFocusable(bool focusable) {}

bool NativeView::IsFocusable() const {
  if (view_)
    return view_->IsFocusable();
  return false;
}

void NativeView::SetBackgroundColor(SkColor color) {
  if (!view_)
    return;
  view_->SetBackground(views::CreateSolidBackground(color));
  view_->SchedulePaint();
}

void NativeView::SetRoundedCorners(
    const NativeView::RoundedCornersOptions& options) {
  rounded_corners_ = options;
  auto* view = GetNative();
  if (!view)
    return;

  view->SetPaintToLayer();
  view->layer()->SetFillsBoundsOpaquely(false);

  // Use rounded corners.
  float radius = options.radius;
  view->layer()->SetRoundedCornerRadius(gfx::RoundedCornersF(
      options.top_left ? radius : 0.0f, options.top_right ? radius : 0.0f,
      options.bottom_right ? radius : 0.0f,
      options.bottom_left ? radius : 0.0f));
  view->layer()->SetIsFastRoundedCorner(true);
}

void NativeView::SetClippingInsets(
    const NativeView::ClippingInsetOptions& options) {
  auto* view = GetNative();
  if (!view)
    return;

  view->SetPaintToLayer();
  view->layer()->SetFillsBoundsOpaquely(false);

  gfx::Rect clip_rect = view->GetLocalBounds();
  gfx::Insets insets = gfx::Insets::TLBR(options.top, options.left,
                                         options.bottom, options.right);
  clip_rect.Inset(insets);
  view->layer()->SetClipRect(clip_rect);
}

void NativeView::ResetScaling() {
  if (view_)
    ResetScalingForView(view_);
}

void NativeView::SetScale(const ScaleAnimationOptions& options) {
  if (view_)
    SetScaleForView(view_, options);
}

float NativeView::GetScaleX() {
  if (view_)
    return GetScaleXForView(view_);
  return 1.0;
}

float NativeView::GetScaleY() {
  if (view_)
    return GetScaleYForView(view_);
  return 1.0;
}

void NativeView::SetOpacity(const double opacity,
                            const AnimationOptions& options) {
  if (view_)
    SetOpacityForView(view_, opacity, options);
}

double NativeView::GetOpacity() {
  if (view_)
    return GetOpacityForView(view_);
  return 1.0;
}

void NativeView::SetClickThrough(bool click_through) {
  is_click_through_ = click_through;
  UpdateClickThrough();
}

bool NativeView::IsClickThrough() const {
  if (is_click_through_)
    return true;
  else if (parent_)
    return parent_->IsClickThrough();
  return false;
}

void NativeView::UpdateClickThrough() {
  auto* view = GetNative();
  if (!view)
    return;

  view->SetCanProcessEventsWithinSubtree(!IsClickThrough());

  for (auto it = children_.begin(); it != children_.end(); it++)
    (*it)->UpdateClickThrough();
}

views::BoundsAnimator* NativeView::GetOrCreateBoundsAnimator() {
  if (!bounds_animator_.get() && view_->parent())
    bounds_animator_ = std::make_unique<views::BoundsAnimator>(view_->parent());
  return bounds_animator_.get();
}

void NativeView::SetBlockScrollViewWhenFocus(bool block) {
  block_scroll_view_when_focus = block;
  UpdateBlockScrollViewWhenFocus(IsBlockScrollViewWhenFocus());
}

bool NativeView::IsBlockScrollViewWhenFocus() const {
  if (block_scroll_view_when_focus)
    return true;
  else if (parent_)
    return parent_->IsBlockScrollViewWhenFocus();
  return false;
}

void NativeView::UpdateBlockScrollViewWhenFocus(bool block) {
  if (!view_)
    return;

  view_->SetProperty(views::kViewBlockScrollViewWhenFocus, block);

  for (auto it = children_.begin(); it != children_.end(); it++)
    (*it)->UpdateBlockScrollViewWhenFocus(block);
}

void NativeView::AddChildViewImpl(NativeView* view) {
  if (!GetNative())
    return;
  GetNative()->AddChildView(view->GetNative());
}

void NativeView::RemoveChildViewImpl(NativeView* view) {
  if (!GetNative())
    return;
  GetNative()->RemoveChildView(view->GetNative());
}

void NativeView::RearrangeChildViews() {
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

}  // namespace electron
