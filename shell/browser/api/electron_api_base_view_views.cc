// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_base_view.h"

#include <algorithm>
#include <queue>
#include <utility>

#include "base/memory/ptr_util.h"
#include "gin/handle.h"
#include "shell/common/color_util.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/background.h"
#include "ui/views/view.h"
#include "ui/views/view_class_properties.h"

namespace electron {

namespace api {

void BaseView::CreateView() {
  SetNativeView(new views::View());
}

void BaseView::OnViewBoundsChanged(views::View* observed_view) {
  if (!view_)
    return;
  gfx::Rect bounds = view_->bounds();
  gfx::Size size = bounds.size();
  gfx::Size old_size = bounds_.size();
  bounds_ = bounds;
  if (size != old_size)
    NotifySizeChanged(old_size, size);
}

void BaseView::OnViewRemovedFromWidget(views::View* observed_view) {
  if (bounds_animator_.get()) {
    bounds_animator_->Cancel();
    bounds_animator_.reset();
  }
}

void BaseView::OnViewIsDeleting(views::View* observed_view) {
  if (view_)
    view_->RemoveObserver(this);
  view_ = nullptr;
  NotifyViewIsDeleting();
}

void BaseView::OnViewHierarchyChanged(
    views::View* observed_view,
    const views::ViewHierarchyChangedDetails& details) {
  SetRoundedCorners(rounded_corners_options_);
  UpdateClickThrough();
  UpdateBlockScrollViewWhenFocus();
}

void BaseView::SetClickThrough(bool click_through) {
  is_click_through_ = click_through;
  UpdateClickThrough();
}

bool BaseView::IsClickThrough() const {
  if (is_click_through_)
    return true;
  else if (parent_)
    return parent_->IsClickThrough();
  return false;
}

void BaseView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  BoundsAnimationOptions options;
  args->GetNext(&options);
  if (!view_)
    return;

  if (!options.animation || !view_->parent()) {
    view_->SetBoundsRect(bounds);
    return;
  }

  if (options.use_from_bounds)
    view_->SetBoundsRect(options.from_bounds);

  if (!bounds_animator_.get())
    bounds_animator_ = std::make_unique<views::BoundsAnimator>(view_->parent());

  bounds_animator_->SetAnimationDuration(
      base::Milliseconds(static_cast<int>(options.duration * 1000)));

  if (!options.use_control_points) {
    gfx::Tween::Type type = gfx::Tween::LINEAR;
    if (options.timing_function == TimingFunction::kLinear)
      type = gfx::Tween::LINEAR;
    else if (options.timing_function == TimingFunction::kEaseIn)
      type = gfx::Tween::EASE_IN;
    else if (options.timing_function == TimingFunction::kEaseOut)
      type = gfx::Tween::EASE_OUT;
    else if (options.timing_function == TimingFunction::kEaseInEaseOut)
      type = gfx::Tween::EASE_IN_OUT;
    else if (options.timing_function == TimingFunction::kDefault)
      type = gfx::Tween::EASE_OUT;
    bounds_animator_->set_tween_type(type);
  } else {
    bounds_animator_->set_cubic_bezier(options.cx1, options.cy1, options.cx2,
                                       options.cy2);
  }

  bounds_animator_->AnimateViewTo(view_, bounds);
}

gfx::Rect BaseView::GetBounds() const {
  if (view_)
    return view_->bounds();
  return gfx::Rect();
}

gfx::Point BaseView::OffsetFromView(gin::Handle<BaseView> from) const {
  if (!view_)
    return gfx::Point();
  gfx::Point point;
  views::View::ConvertPointToTarget(from->GetView(), view_, &point);
  return point;
}

gfx::Point BaseView::OffsetFromWindow() const {
  if (!view_)
    return gfx::Point();
  gfx::Point point;
  views::View::ConvertPointFromWidget(view_, &point);
  return point;
}

void BaseView::SetVisible(bool visible) {
  if (visible == IsVisible())
    return;
  if (view_)
    view_->SetVisible(visible);
}

bool BaseView::IsVisible() const {
  if (view_)
    return view_->GetVisible();
  return false;
}

bool BaseView::IsTreeVisible() const {
  return IsVisible();
}

void BaseView::Focus() {
  if (view_)
    view_->RequestFocus();
}

bool BaseView::HasFocus() const {
  if (view_)
    return view_->HasFocus();
  return false;
}

void BaseView::SetFocusable(bool focusable) {}

bool BaseView::IsFocusable() const {
  if (view_)
    return view_->IsFocusable();
  return false;
}

void BaseView::SetBackgroundColor(const std::string& color_name) {
  const SkColor color = ParseCSSColor(color_name);
  if (view_) {
    view_->SetBackground(views::CreateSolidBackground(color));
    view_->SchedulePaint();
  }
  SetBackgroundColorImpl(color);
}

void BaseView::SetRoundedCorners(const RoundedCornersOptions& options) {
  if (!view_)
    return;

  rounded_corners_options_ = options;
  view_->SetPaintToLayer();
  view_->layer()->SetFillsBoundsOpaquely(false);

  // Use rounded corners.
  float radius = options.radius;
  view_->layer()->SetRoundedCornerRadius(gfx::RoundedCornersF(
      options.top_left ? radius : 0.0f, options.top_right ? radius : 0.0f,
      options.bottom_right ? radius : 0.0f,
      options.bottom_left ? radius : 0.0f));
  view_->layer()->SetIsFastRoundedCorner(true);
}

void BaseView::SetClippingInsets(const ClippingInsetOptions& options) {
  if (!view_)
    return;

  view_->SetPaintToLayer();
  view_->layer()->SetFillsBoundsOpaquely(false);

  gfx::Rect clip_rect = view_->GetLocalBounds();
  gfx::Insets insets = gfx::Insets::TLBR(options.top, options.left,
                                         options.bottom, options.right);
  clip_rect.Inset(insets);
  view_->layer()->SetClipRect(clip_rect);
}

void BaseView::ResetScaling() {}

void BaseView::SetScale(const ScaleAnimationOptions& options) {}

float BaseView::GetScaleX() const {
  return 1.0;
}

float BaseView::GetScaleY() const {
  return 1.0;
}

void BaseView::SetOpacity(const double opacity, gin::Arguments* args) {
  AnimationOptions options;
  args->GetNext(&options);
}

double BaseView::GetOpacity() const {
  return 1.0;
}

void BaseView::RearrangeChildViews() {
  if (api_children_.size() == 0)
    return;

  std::sort(api_children_.begin(), api_children_.end(),
            [](auto* a, auto* b) { return a->GetZIndex() < b->GetZIndex(); });

  auto begin = api_children_.begin();
  auto* first = *begin;
  begin++;

  for (auto it = begin; it != api_children_.end(); it++) {
    auto* second = *it;

    int index_of_first = view_->GetIndexOf(first->GetView());
    int index_of_second = view_->GetIndexOf(second->GetView());
    if (index_of_second != index_of_first + 1)
      view_->ReorderChildView(second->GetView(), index_of_first + 1);

    first = second;
  }
}

void BaseView::SetNativeView(NATIVEVIEW view) {
  if (view_) {
    view_->RemoveObserver(this);
    if (delete_view_)
      delete view_;
  }

  view_ = view;
  view_->AddObserver(this);
}

void BaseView::DestroyView() {
  if (!view_)
    return;
  view_->RemoveObserver(this);
  if (delete_view_)
    delete view_;
  view_ = nullptr;
}

void BaseView::AddChildViewImpl(BaseView* view) {
  if (!view_)
    return;
  view->GetView()->set_owned_by_client();
  view_->AddChildView(view->GetView());
}

void BaseView::RemoveChildViewImpl(BaseView* view) {
  if (!view_)
    return;
  view_->RemoveChildView(view->GetView());
}

void BaseView::UpdateClickThrough() {
  if (!view_)
    return;

  view_->SetCanProcessEventsWithinSubtree(!IsClickThrough());

  for (auto* child : api_children_)
    child->UpdateClickThrough();
}

void BaseView::SetBlockScrollViewWhenFocus(bool block) {
  block_scroll_view_when_focus = block;
  UpdateBlockScrollViewWhenFocus();
}

bool BaseView::IsBlockScrollViewWhenFocus() const {
  if (block_scroll_view_when_focus)
    return true;
  else if (parent_)
    return parent_->IsBlockScrollViewWhenFocus();
  return false;
}

void BaseView::UpdateBlockScrollViewWhenFocus() {
  if (!view_)
    return;

  bool block = IsBlockScrollViewWhenFocus();

  if (!api_children_.empty()) {
    view_->SetProperty(views::kViewBlockScrollViewWhenFocus, block);
    for (auto it = api_children_.begin(); it != api_children_.end(); it++)
      (*it)->UpdateBlockScrollViewWhenFocus();
    return;
  }

  std::queue<views::View*> q;
  q.push(GetView());
  while (!q.empty()) {
    auto* view = q.front();
    q.pop();
    view->SetProperty(views::kViewBlockScrollViewWhenFocus, block);
    for (auto* child : view->children())
      q.push(child);
  }
}

}  // namespace api

}  // namespace electron
