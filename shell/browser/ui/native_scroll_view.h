#ifndef SHELL_BROWSER_UI_NATIVE_SCROLL_VIEW_H_
#define SHELL_BROWSER_UI_NATIVE_SCROLL_VIEW_H_

#include "base/callback.h"
#include "build/build_config.h"
#include "build/buildflag.h"

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
#include "base/callback_list.h"
#include "ui/compositor/compositor_observer.h"
#endif  // defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)

#include "shell/browser/ui/native_view.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace electron {

enum class ScrollBarMode { kDisabled, kHiddenButEnabled, kEnabled };

enum class ScrollElasticity {
  kAutomatic = 0,  // NSScrollElasticityAutomatic = 0
  kNone = 1,       // NSScrollElasticityNone = 1
  kAllowed = 2,    // NSScrollElasticityAllowed = 2
};

class NativeScrollView : public NativeView {
 public:
  NativeScrollView(absl::optional<ScrollBarMode> horizontal_mode,
                   absl::optional<ScrollBarMode> vertical_mode);

  // NativeView:
  void SetBackgroundColor(SkColor color) override;
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  void UpdateClickThrough() override;
#endif
  void DetachChildView(NativeView* view) override;

  void SetContentView(scoped_refptr<NativeView> view);
  NativeView* GetContentView() const;

  void SetContentSize(const gfx::Size& size);
  gfx::Size GetContentSize() const;

  void SetScrollPosition(gfx::Point point,
      base::OnceCallback<void(std::string)> callback);
  gfx::Point GetScrollPosition() const;
  gfx::Point GetMaximumScrollPosition() const;

  void SetHorizontalScrollBarMode(ScrollBarMode mode);
  ScrollBarMode GetHorizontalScrollBarMode() const;
  void SetVerticalScrollBarMode(ScrollBarMode mode);
  ScrollBarMode GetVerticalScrollBarMode() const;
  void SetScrollWheelSwapped(bool swap);
  bool IsScrollWheelSwapped();
  void SetScrollEventsEnabled(bool enable);
  bool IsScrollEventsEnabled();
  void SetHorizontalScrollElasticity(ScrollElasticity elasticity);
  ScrollElasticity GetHorizontalScrollElasticity() const;
  void SetVerticalScrollElasticity(ScrollElasticity elasticity);
  ScrollElasticity GetVerticalScrollElasticity() const;

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
  void SetSmoothScroll(bool enable);

  void OnDidScroll();
#endif

 protected:
  ~NativeScrollView() override;

  // NativeView:
  void SetWindowForChildren(NativeWindow* window) override;

  void InitScrollView(absl::optional<ScrollBarMode> horizontal_mode,
                      absl::optional<ScrollBarMode> vertical_mode);
  void SetContentViewImpl(NativeView* container);
  void DetachChildViewImpl();

 private:
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  class CompositorObserver : public ui::CompositorObserver {
   public:
    CompositorObserver(NativeScrollView* native_scroll_view);
    ~CompositorObserver() override;

    void SetScrollPosition(gfx::Point point,
        base::OnceCallback<void(std::string)> callback);

    void OnCompositingDidCommit(ui::Compositor* compositor) override;

    bool is_inside_set_scroll_position() const {
      return is_inside_set_scroll_position_;
    }

    gfx::Point* point() { return point_.get(); }

   private:
    NativeScrollView* native_scroll_view_;
    std::unique_ptr<gfx::Point> point_;
    base::OnceCallback<void(std::string)> completion_callback_;
    bool is_inside_set_scroll_position_;
  };

  std::unique_ptr<CompositorObserver> compositor_observer_;
  bool set_scroll_position_after_commit_ = false;
  bool smooth_scroll_ = false;
  bool scroll_events_ = false;
  base::CallbackListSubscription on_contents_scrolled_subscription_;
#endif  // defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)

  scoped_refptr<NativeView> content_view_;
};

}  // namespace electron

#endif  // SHELL_BROWSER_UI_NATIVE_SCROLL_VIEW_H_
