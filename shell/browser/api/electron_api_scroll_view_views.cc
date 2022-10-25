// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_scroll_view.h"

#include "base/cxx17_backports.h"
#include "cc/layers/layer.h"
#include "shell/browser/browser.h"
#include "shell/browser/ui/views/scroll/scroll_bar_views.h"
#include "shell/common/color_util.h"
#include "ui/base/ui_base_features.h"
#include "ui/compositor/compositor.h"
#include "ui/compositor/layer.h"
#include "ui/views/controls/native/native_view_host.h"
#include "ui/views/controls/scroll_view.h"
#include "ui/views/widget/widget.h"

#if !BUILDFLAG(IS_MAC)
#include "shell/browser/ui/views/scroll/scroll_view.h"
#include "shell/browser/ui/views/scroll/scroll_view_scroll_with_layers.h"
#endif

namespace electron::api {

namespace {

void UpdateScrollBars(views::ScrollView* scroll_view, bool is_smooth_scroll) {
  if (!scroll_view)
    return;

  if (is_smooth_scroll) {
    scroll_view->SetHorizontalScrollBar(std::make_unique<ScrollBarViews>(true));
    scroll_view->SetVerticalScrollBar(std::make_unique<ScrollBarViews>(false));
  } else {
    scroll_view->SetHorizontalScrollBar(
        std::make_unique<views::ScrollBarViews>(true));
    scroll_view->SetVerticalScrollBar(
        std::make_unique<views::ScrollBarViews>(false));
  }
}

}  // namespace

ScrollView::CompositorObserver::CompositorObserver(ScrollView* scroll_view)
    : scroll_view_(scroll_view), is_inside_set_scroll_position_(false) {
  DCHECK(scroll_view_);

  if (!scroll_view_->GetView())
    return;

  if (!scroll_view_->GetView()->GetWidget())
    return;

  if (!scroll_view_->GetView()->GetWidget()->GetCompositor())
    return;

  scroll_view_->GetView()->GetWidget()->GetCompositor()->AddObserver(this);
}

ScrollView::CompositorObserver::~CompositorObserver() {
  if (!scroll_view_->GetView())
    return;

  if (!scroll_view_->GetView()->GetWidget())
    return;

  if (!scroll_view_->GetView()->GetWidget()->GetCompositor())
    return;

  scroll_view_->GetView()->GetWidget()->GetCompositor()->RemoveObserver(this);
}

void ScrollView::CompositorObserver::SetScrollPosition(
    gfx::Point point,
    base::OnceCallback<void(std::string)> callback) {
  point_ = std::make_unique<gfx::Point>(point);
  completion_callback_ = std::move(callback);

  views::ScrollView* scroll =
      static_cast<views::ScrollView*>(scroll_view_->GetView());

  if (!scroll)
    return;

  views::View* contents_view = scroll->contents();
  if (!contents_view)
    return;

  ui::Layer* contents_layer = contents_view->layer();
  if (!contents_layer)
    return;

  cc::Layer* contents_cc_layer = contents_layer->cc_layer_for_testing();
  if (!contents_cc_layer)
    return;

  contents_cc_layer->SetNeedsCommit();
}

void ScrollView::CompositorObserver::OnCompositingDidCommit(
    ui::Compositor* compositor) {
  if (!point_)
    return;

  is_inside_set_scroll_position_ = true;
  scroll_view_->SetScrollPositionImpl(*point_, std::move(completion_callback_));
  is_inside_set_scroll_position_ = false;
  point_.reset();
}

void ScrollView::SetBackgroundColor(const std::string& color_name) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    BaseView::SetBackgroundColorMac(color_name);
    return;
  }
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  scroll->SetBackgroundColor(ParseCSSColor(color_name));
}

void ScrollView::UpdateClickThrough() {
  BaseView::UpdateClickThrough();

  if (api_content_view_)
    api_content_view_->UpdateClickThrough();
}

void ScrollView::SetContentSize(const gfx::Size& size) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetContentSizeMac(size);
    return;
  }
#endif
  if (!api_content_view_)
    return;
  api_content_view_->GetView()->SetSize(size);
}

void ScrollView::SetHorizontalScrollBarMode(std::string mode) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetHorizontalScrollBarModeMac(mode);
    return;
  }
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  views::ScrollView::ScrollBarMode scroll_bar_mode =
      views::ScrollView::ScrollBarMode::kEnabled;
  if (mode == "disabled")
    scroll_bar_mode = views::ScrollView::ScrollBarMode::kDisabled;
  else if (mode == "enabled-but-hidden")
    scroll_bar_mode = views::ScrollView::ScrollBarMode::kHiddenButEnabled;
  scroll->SetHorizontalScrollBarMode(scroll_bar_mode);
}

std::string ScrollView::GetHorizontalScrollBarMode() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetHorizontalScrollBarModeMac();
#endif
  if (!GetView())
    return "disabled";
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  auto mode = scroll->GetHorizontalScrollBarMode();
  if (mode == views::ScrollView::ScrollBarMode::kDisabled)
    return "disabled";
  else if (mode == views::ScrollView::ScrollBarMode::kHiddenButEnabled)
    return "enabled-but-hidden";
  return "enabled";
}

void ScrollView::SetVerticalScrollBarMode(std::string mode) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetVerticalScrollBarModeMac(mode);
    return;
  }
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  views::ScrollView::ScrollBarMode scroll_bar_mode =
      views::ScrollView::ScrollBarMode::kEnabled;
  if (mode == "disabled")
    scroll_bar_mode = views::ScrollView::ScrollBarMode::kDisabled;
  else if (mode == "enabled-but-hidden")
    scroll_bar_mode = views::ScrollView::ScrollBarMode::kHiddenButEnabled;
  scroll->SetVerticalScrollBarMode(scroll_bar_mode);
}

std::string ScrollView::GetVerticalScrollBarMode() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetVerticalScrollBarModeMac();
#endif
  if (!GetView())
    return "disabled";
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  auto mode = scroll->GetVerticalScrollBarMode();
  if (mode == views::ScrollView::ScrollBarMode::kDisabled)
    return "disabled";
  else if (mode == views::ScrollView::ScrollBarMode::kHiddenButEnabled)
    return "enabled-but-hidden";
  return "enabled";
}

void ScrollView::SetScrollWheelSwapped(bool swap) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetScrollWheelSwappedMac(swap);
    return;
  }
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  scroll->SetTreatAllScrollEventsAsHorizontal(swap);
}

bool ScrollView::IsScrollWheelSwapped() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return IsScrollWheelSwappedMac();
#endif
  if (!GetView())
    return false;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetTreatAllScrollEventsAsHorizontal();
}

void ScrollView::SetScrollEventsEnabled(bool enable) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetScrollEventsEnabledMac(enable);
    return;
  }
#endif
  scroll_events_ = enable;
}

bool ScrollView::IsScrollEventsEnabled() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return IsScrollEventsEnabledMac();
#endif
  return scroll_events_;
}

void ScrollView::SetHorizontalScrollElasticity(std::string elasticity) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    SetHorizontalScrollElasticityMac(elasticity);
#endif
}

std::string ScrollView::GetHorizontalScrollElasticity() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetHorizontalScrollElasticityMac();
#endif
  return "none";
}

void ScrollView::SetVerticalScrollElasticity(std::string elasticity) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    SetVerticalScrollElasticityMac(elasticity);
#endif
}

std::string ScrollView::GetVerticalScrollElasticity() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetVerticalScrollElasticityMac();
#endif
  return "none";
}

gfx::Point ScrollView::GetScrollPosition() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetScrollPositionMac();
#endif
  if (!GetView())
    return gfx::Point();

  if (set_scroll_position_after_commit_ && compositor_observer_ &&
      compositor_observer_->point()) {
    return *compositor_observer_->point();
  }

  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetVisibleRect().origin();
}

gfx::Point ScrollView::GetMaximumScrollPosition() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return GetMaximumScrollPositionMac();
#endif
  if (!GetView())
    return gfx::Point();
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  gfx::Size content_size = scroll->contents()->bounds().size();
  gfx::Size viewport_size = scroll->GetVisibleRect().size();
  return gfx::Point(
      std::max(0, content_size.width() - viewport_size.width()),
      std::max(0, content_size.height() - viewport_size.height()));
}

void ScrollView::ClipHeightTo(int min_height, int max_height) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  scroll->ClipHeightTo(min_height, max_height);
}

int ScrollView::GetMinHeight() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return 0;
#endif
  if (!GetView())
    return 0;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetMinHeight();
}

int ScrollView::GetMaxHeight() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return 0;
#endif
  if (!GetView())
    return 0;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetMaxHeight();
}

void ScrollView::ScrollRectToVisible(const gfx::Rect& rect) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  if (!api_content_view_)
    return;
  api_content_view_->GetView()->ScrollRectToVisible(rect);
}

gfx::Rect ScrollView::GetVisibleRect() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return gfx::Rect();
#endif
  if (!GetView())
    return gfx::Rect();
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetVisibleRect();
}

void ScrollView::SetAllowKeyboardScrolling(bool allow) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  scroll->SetAllowKeyboardScrolling(allow);
}

bool ScrollView::GetAllowKeyboardScrolling() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return false;
#endif
  if (!GetView())
    return false;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetAllowKeyboardScrolling();
}

void ScrollView::SetDrawOverflowIndicator(bool indicator) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  if (!GetView())
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  scroll->SetDrawOverflowIndicator(indicator);
}

bool ScrollView::GetDrawOverflowIndicator() const {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return false;
#endif
  if (!GetView())
    return false;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  return scroll->GetDrawOverflowIndicator();
}

void ScrollView::CreateScrollView() {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    CreateScrollViewMac();
    return;
  }
#endif
  views::ScrollView* scroll_view = nullptr;
#if BUILDFLAG(IS_MAC)
  scroll_view = new views::ScrollView();
#else
  if (base::FeatureList::IsEnabled(::features::kUiCompositorScrollWithLayers)) {
    scroll_view = new ScrollViewScrollWithLayers();
    set_scroll_position_after_commit_ = true;
  } else {
    scroll_view = new electron::ScrollView();
  }
#endif

  scroll_view->SetBackgroundColor(absl::optional<SkColor>());

  on_contents_scrolled_subscription_ = scroll_view->AddContentsScrolledCallback(
      base::BindRepeating(&ScrollView::OnDidScroll, base::Unretained(this)));

  SetView(scroll_view);
}

void ScrollView::SetSmoothScroll(bool enable) {
  if (smooth_scroll_ != enable && GetView()) {
    auto* scroll = static_cast<views::ScrollView*>(GetView());
    UpdateScrollBars(scroll, enable);
  }
  smooth_scroll_ = enable;
}

void ScrollView::OnDidScroll() {
  if (IsScrollEventsEnabled())
    NotifyDidScroll();
}

void ScrollView::SetContentViewImpl(BaseView* view) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetContentViewImplMac(view);
    return;
  }
#endif
  if (!GetView() || !view)
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  UpdateScrollBars(scroll, smooth_scroll_);
  view->set_delete_view(false);
  auto content_view = std::unique_ptr<views::View>(view->GetView());
  scroll->SetContents(std::move(content_view));
}

void ScrollView::ResetCurrentContentViewImpl() {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    ResetCurrentContentViewImplMac();
    return;
  }
#endif
  if (!GetView() || !api_content_view_)
    return;
  auto* scroll = static_cast<views::ScrollView*>(GetView());
  DCHECK_EQ(scroll->contents(), api_content_view_->GetView());
  scroll->SetContents(nullptr);
  api_content_view_->set_delete_view(true);
}

void ScrollView::SetScrollPositionImpl(
    gfx::Point point,
    base::OnceCallback<void(std::string)> callback) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    SetScrollPositionImplMac(point, std::move(callback));
    return;
  }
#endif
  if (!GetView() || !api_content_view_) {
    std::move(callback).Run(std::string("Error"));
    return;
  }

  auto* scroll = static_cast<views::ScrollView*>(GetView());
  gfx::Size content_size = scroll->contents()->bounds().size();
  gfx::Rect visible_rect = scroll->GetVisibleRect();
  int max_x_position = std::max(0, content_size.width() - visible_rect.width());
  int max_y_position =
      std::max(0, content_size.height() - visible_rect.height());
  point.set_x(base::clamp(point.x(), 0, max_x_position));
  point.set_y(base::clamp(point.y(), 0, max_y_position));

  if (set_scroll_position_after_commit_) {
    if (!compositor_observer_) {
      compositor_observer_ =
          std::make_unique<ScrollView::CompositorObserver>(this);
    }

    if (!compositor_observer_->is_inside_set_scroll_position()) {
      compositor_observer_->SetScrollPosition(point, std::move(callback));
      return;
    }
  }

  // If a scrollBar is disabled, then we need to enable it when performing
  // ScrollView::ScrollToOffset, otherwise updating scrollBar positions is not
  // executed.
  auto horiz_mode = scroll->GetHorizontalScrollBarMode();
  if (horiz_mode == views::ScrollView::ScrollBarMode::kDisabled)
    scroll->SetHorizontalScrollBarMode(
        views::ScrollView::ScrollBarMode::kHiddenButEnabled);
  auto vert_mode = scroll->GetHorizontalScrollBarMode();
  if (vert_mode == views::ScrollView::ScrollBarMode::kDisabled)
    scroll->SetVerticalScrollBarMode(
        views::ScrollView::ScrollBarMode::kHiddenButEnabled);

  api_content_view_->GetView()->ScrollRectToVisible(gfx::Rect(
      point.x(), point.y(), visible_rect.width(), visible_rect.height()));

  if (horiz_mode == views::ScrollView::ScrollBarMode::kDisabled)
    scroll->SetHorizontalScrollBarMode(horiz_mode);
  if (vert_mode == views::ScrollView::ScrollBarMode::kDisabled)
    scroll->SetVerticalScrollBarMode(vert_mode);

  std::move(callback).Run(std::string());

  scroll->Layout();
}

}  // namespace electron::api
