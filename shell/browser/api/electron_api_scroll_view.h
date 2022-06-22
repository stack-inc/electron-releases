#ifndef SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_
#define SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_

#include "shell/browser/api/electron_api_base_view.h"
#include "shell/browser/ui/native_scroll_view.h"

namespace electron {

namespace api {

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
  ScrollView(gin::Arguments* args, NativeScrollView* scroll);
  ~ScrollView() override;

  // BaseView:
  void ResetChildView(BaseView* view) override;
  void ResetChildViews() override;

  // NativeView::Observer:
#if BUILDFLAG(IS_MAC)
  void OnWillStartLiveScroll(NativeView* observed_view) override;
  void OnDidLiveScroll(NativeView* observed_view) override;
  void OnDidEndLiveScroll(NativeView* observed_view) override;
  void OnScrollWheel(NativeView* observed_view,
                     bool mouse_event,
                     float scrolling_delta_x,
                     float scrolling_delta_y,
                     std::string phase,
                     std::string momentum_phase) override;
#endif  // BUILDFLAG(IS_MAC)

#if defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_MAC)
  void OnDidScroll(NativeView* observed_view) override;
#endif  // defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_MAC)

  void SetContentView(v8::Local<v8::Value> value);
  v8::Local<v8::Value> GetContentView() const;
  void SetContentSize(gfx::Size size);
  gfx::Size GetContentSize() const;
  void SetHorizontalScrollBarMode(std::string mode);
  std::string GetHorizontalScrollBarMode() const;
  void SetVerticalScrollBarMode(std::string mode);
  std::string GetVerticalScrollBarMode() const;
  void SetScrollWheelSwapped(bool swap);
  bool IsScrollWheelSwapped();
#if BUILDFLAG(IS_MAC)
  void SetHorizontalScrollElasticity(std::string elasticity);
  std::string GetHorizontalScrollElasticity() const;
  void SetVerticalScrollElasticity(std::string elasticity);
  std::string GetVerticalScrollElasticity() const;
#endif
  void SetScrollPosition(gfx::Point point);
  gfx::Point GetScrollPosition() const;
  gfx::Point GetMaximumScrollPosition() const;
#if BUILDFLAG(IS_MAC)
  void ScrollToPoint(gfx::Point point, const AnimationOptions& options);
  void ScrollPointToCenter(gfx::Point point, const AnimationOptions& options);
  void SetOverlayScrollbar(bool overlay);
  bool IsOverlayScrollbar() const;
  void SetScrollWheelFactor(double factor);
  double GetScrollWheelFactor();
#endif
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  void ClipHeightTo(int min_height, int max_height);
  int GetMinHeight() const;
  int GetMaxHeight() const;
  void ScrollRectToVisible(const gfx::Rect& rect);
  gfx::Rect GetVisibleRect() const;
  void SetAllowKeyboardScrolling(bool allow);
  bool GetAllowKeyboardScrolling() const;
  void SetDrawOverflowIndicator(bool indicator);
  bool GetDrawOverflowIndicator() const;
#endif
#if defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_MAC)
  void SetScrollEventsEnabled(bool enable);
  bool IsScrollEventsEnabled();
#endif  // defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_MAC)

 private:
  NativeScrollView* scroll_;

  int32_t content_view_id_ = 0;
  v8::Global<v8::Value> content_view_;
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_BROWSER_API_ELECTRON_API_SCROLL_VIEW_H_
