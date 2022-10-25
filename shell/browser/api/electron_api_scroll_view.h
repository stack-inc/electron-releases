// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_
#define ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_

#include "base/callback_list.h"
#include "shell/browser/api/electron_api_base_view.h"
#include "ui/compositor/compositor_observer.h"

namespace gin_helper {
class Dictionary;
}

namespace electron::api {

class ScrollView : public BaseView {
 public:
  static gin_helper::WrappableBase* New(gin_helper::ErrorThrower thrower,
                                        gin::Arguments* args);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  // disable copy
  ScrollView(const ScrollView&) = delete;
  ScrollView& operator=(const ScrollView&) = delete;

 protected:
  ScrollView(gin::Arguments* args, const gin_helper::Dictionary& options);
  ~ScrollView() override;

  // BaseView:
  void SetBackgroundColor(const std::string& color_name) override;
  void UpdateClickThrough() override;
  void RemoveChildView(gin::Handle<BaseView> base_view) override;
  void ResetChildViews() override;
  void SetWindow(BaseWindow* window) override;

  // ScrollView APIs.
  void SetContentView(gin::Handle<BaseView> base_view);
  v8::Local<v8::Value> GetContentView() const;
  void SetContentSize(const gfx::Size& size);
  gfx::Size GetContentSize() const;
  void SetHorizontalScrollBarMode(std::string mode);
  std::string GetHorizontalScrollBarMode() const;
  void SetVerticalScrollBarMode(std::string mode);
  std::string GetVerticalScrollBarMode() const;
  void SetScrollWheelSwapped(bool swap);
  bool IsScrollWheelSwapped() const;
  void SetScrollEventsEnabled(bool enable);
  bool IsScrollEventsEnabled() const;
  void SetHorizontalScrollElasticity(std::string elasticity);
  std::string GetHorizontalScrollElasticity() const;
  void SetVerticalScrollElasticity(std::string elasticity);
  std::string GetVerticalScrollElasticity() const;
  v8::Local<v8::Promise> SetScrollPosition(gfx::Point point);
  gfx::Point GetScrollPosition() const;
  gfx::Point GetMaximumScrollPosition() const;
#if BUILDFLAG(IS_MAC)
  void ScrollToPoint(gfx::Point point, const AnimationOptions& options);
  void ScrollPointToCenter(gfx::Point point, const AnimationOptions& options);
  void SetOverlayScrollbar(bool overlay);
  bool IsOverlayScrollbar() const;
  void SetScrollWheelFactor(double factor);
  double GetScrollWheelFactor() const;
#endif
  void ClipHeightTo(int min_height, int max_height);
  int GetMinHeight() const;
  int GetMaxHeight() const;
  void ScrollRectToVisible(const gfx::Rect& rect);
  gfx::Rect GetVisibleRect() const;
  void SetAllowKeyboardScrolling(bool allow);
  bool GetAllowKeyboardScrolling() const;
  void SetDrawOverflowIndicator(bool indicator);
  bool GetDrawOverflowIndicator() const;

  // Helpers.

  void CreateScrollView();

  void SetSmoothScroll(bool enable);
  void OnDidScroll();

  void SetContentViewImpl(BaseView* view);
  void ResetCurrentContentViewImpl();

  void SetScrollPositionImpl(gfx::Point point,
                             base::OnceCallback<void(std::string)> callback);

#if BUILDFLAG(IS_MAC)
  void SetContentSizeMac(const gfx::Size& size);
  void SetHorizontalScrollBarModeMac(std::string mode);
  std::string GetHorizontalScrollBarModeMac() const;
  void SetVerticalScrollBarModeMac(std::string mode);
  std::string GetVerticalScrollBarModeMac() const;
  void SetScrollWheelSwappedMac(bool swap);
  bool IsScrollWheelSwappedMac() const;
  void SetScrollEventsEnabledMac(bool enable);
  bool IsScrollEventsEnabledMac() const;
  void SetHorizontalScrollElasticityMac(std::string elasticity);
  std::string GetHorizontalScrollElasticityMac() const;
  void SetVerticalScrollElasticityMac(std::string elasticity);
  std::string GetVerticalScrollElasticityMac() const;
  gfx::Point GetScrollPositionMac() const;
  gfx::Point GetMaximumScrollPositionMac() const;
  void CreateScrollViewMac();
  void SetContentViewImplMac(BaseView* view);
  void ResetCurrentContentViewImplMac();
  void SetScrollPositionImplMac(gfx::Point point,
                                base::OnceCallback<void(std::string)> callback);
#endif  // BUILDFLAG(IS_MAC)

 public:
#if BUILDFLAG(IS_MAC)
  void NotifyWillStartLiveScroll();
  void NotifyDidLiveScroll();
  void NotifyDidEndLiveScroll();
  void NotifyScrollWheel(bool mouse_event,
                         float scrolling_delta_x,
                         float scrolling_delta_y,
                         std::string phase,
                         std::string momentum_phase);
#endif  // BUILDFLAG(IS_MAC)

  void NotifyDidScroll();

 private:
  class CompositorObserver : public ui::CompositorObserver {
   public:
    CompositorObserver(ScrollView* scroll_view);
    ~CompositorObserver() override;

    bool is_inside_set_scroll_position() const {
      return is_inside_set_scroll_position_;
    }

    gfx::Point* point() { return point_.get(); }

    void SetScrollPosition(gfx::Point point,
                           base::OnceCallback<void(std::string)> callback);

    // ui::CompositorObserver:
    void OnCompositingDidCommit(ui::Compositor* compositor) override;

   private:
    ScrollView* scroll_view_;
    std::unique_ptr<gfx::Point> point_;
    base::OnceCallback<void(std::string)> completion_callback_;
    bool is_inside_set_scroll_position_;
  };

  std::unique_ptr<CompositorObserver> compositor_observer_;
  bool set_scroll_position_after_commit_ = false;
  bool smooth_scroll_ = false;
  bool scroll_events_ = false;
  base::CallbackListSubscription on_contents_scrolled_subscription_;

  v8::Global<v8::Value> content_view_;
  BaseView* api_content_view_ = nullptr;
};

}  // namespace electron::api

#endif  // ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_
