#ifndef SHELL_BROWSER_UI_NATIVE_VIEW_H_
#define SHELL_BROWSER_UI_NATIVE_VIEW_H_

#include <vector>

#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "base/observer_list_types.h"
#include "base/supports_user_data.h"
#include "shell/browser/ui/view_utils.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
#include "ui/views/animation/bounds_animator.h"
#include "ui/views/view_observer.h"
#endif

#if BUILDFLAG(IS_MAC)
#ifdef __OBJC__
@class NSEvent;
@class NSView;
#else
class NSEvent;
struct NSView;
#endif
#elif defined(TOOLKIT_VIEWS)
namespace views {
class View;
}
#endif

namespace electron {

#if BUILDFLAG(IS_MAC)
using NATIVEEVENT = NSEvent*;
using NATIVEVIEW = NSView*;
#elif defined(TOOLKIT_VIEWS)
using NATIVEVIEW = views::View*;
#endif

class NativeWindow;

#if BUILDFLAG(IS_MAC)
// Values here should match NSVisualEffectMaterial.
enum class VisualEffectMaterial {
  kAppearanceBased,
  kLight,
  kDark,
  kTitlebar,
};

// Values here should match NSVisualEffectBlendingMode.
enum class VisualEffectBlendingMode {
  kBehindWindow,
  kWithinWindow,
};

// Supported event types.
enum class EventType {
  kUnknown,
  kLeftMouseDown,
  kRightMouseDown,
  kOtherMouseDown,
  kLeftMouseUp,
  kRightMouseUp,
  kOtherMouseUp,
  kMouseMove,
  kMouseEnter,
  kMouseLeave,
};

// Base event type.
struct NativeEvent {
  EventType type;

  // Time when event was created, starts from when machine was booted.
  uint32_t timestamp;

  // The underlying native event.
  NATIVEEVENT native_event;

 protected:
  NativeEvent(NATIVEEVENT event, NATIVEVIEW view);
};

struct NativeMouseEvent : public NativeEvent {
  // Create from the native event.
  NativeMouseEvent(NATIVEEVENT event, NATIVEVIEW view);

  int button;
  gfx::Point position_in_view;
  gfx::Point position_in_window;
};
#endif  // BUILDFLAG(IS_MAC)

// The base class for all kinds of views.
class NativeView : public base::RefCounted<NativeView>,
                   public base::SupportsUserData
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
    ,
                   public views::ViewObserver
#endif
{
 public:
  class Observer : public base::CheckedObserver {
   public:
    ~Observer() override {}

    virtual void OnChildViewDetached(NativeView* observed_view,
                                     NativeView* view) {}
#if BUILDFLAG(IS_MAC)
    virtual bool OnMouseDown(NativeView* observed_view,
                             const NativeMouseEvent& event);
    virtual bool OnMouseUp(NativeView* observed_view,
                           const NativeMouseEvent& event);
    virtual void OnMouseMove(NativeView* observed_view,
                             const NativeMouseEvent& event) {}
    virtual void OnMouseEnter(NativeView* observed_view,
                              const NativeMouseEvent& event) {}
    virtual void OnMouseLeave(NativeView* observed_view,
                              const NativeMouseEvent& event) {}
    virtual void OnCaptureLost(NativeView* observed_view) {}
    virtual void OnWillStartLiveScroll(NativeView* observed_view) {}
    virtual void OnDidLiveScroll(NativeView* observed_view) {}
    virtual void OnDidEndLiveScroll(NativeView* observed_view) {}
    virtual void OnScrollWheel(NativeView* observed_view,
                               bool mouse_event,
                               float scrolling_delta_x,
                               float scrolling_delta_y,
                               std::string phase,
                               std::string momentum_phase) {}
#endif  // BUILDFLAG(IS_MAC)

    virtual void OnDidScroll(NativeView* observed_view) {}

    virtual void OnSizeChanged(NativeView* observed_view,
                               gfx::Size old_size,
                               gfx::Size new_size) {}

    virtual void OnViewIsDeleting(NativeView* observed_view) {}
  };

  struct RoundedCornersOptions {
    RoundedCornersOptions();

    float radius = 0.f;
    bool top_left = false;
    bool top_right = false;
    bool bottom_left = false;
    bool bottom_right = false;
  };

  struct ClippingInsetOptions {
    ClippingInsetOptions();

    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;
  };

  NativeView();
  NativeView(bool vibrant, bool blurred);

  // Change position and size.
  virtual void SetBounds(const gfx::Rect& bounds,
                         const BoundsAnimationOptions& options);

  // Get position and size.
  gfx::Rect GetBounds() const;

  // Coordinate convertions.
  gfx::Point OffsetFromView(const NativeView* from) const;
  gfx::Point OffsetFromWindow() const;

  // Show/Hide the view.
  void SetVisible(bool visible);
  bool IsVisible() const;

  // Whether the view and its parent are visible.
  bool IsTreeVisible() const;

  // Move the keyboard focus to the view.
  void Focus();
  bool HasFocus() const;

  // Whether the view can be focused.
  void SetFocusable(bool focusable);
  bool IsFocusable() const;

  // Display related styles.
  virtual void SetBackgroundColor(SkColor color);

#if BUILDFLAG(IS_MAC)
  void SetVisualEffectMaterial(VisualEffectMaterial material);
  VisualEffectMaterial GetVisualEffectMaterial() const;
  void SetVisualEffectBlendingMode(VisualEffectBlendingMode mode);
  VisualEffectBlendingMode GetVisualEffectBlendingMode() const;

  void SetBlurTintColorWithSRGB(float r, float g, float b, float a);
  void SetBlurTintColorWithCalibratedWhite(float white, float alphaval);
  void SetBlurTintColorWithGenericGamma22White(float white, float alphaval);
  void SetBlurRadius(float radius);
  float GetBlurRadius();
  void SetBlurSaturationFactor(float factor);
  float GetBlurSaturationFactor();

  // Capture mouse.
  void SetCapture();
  void ReleaseCapture();
  bool HasCapture() const;

  void EnableMouseEvents();
  void SetMouseTrackingEnabled(bool enable);
  bool IsMouseTrackingEnabled();

  void SetWantsLayer(bool wants);
  bool WantsLayer() const;
#endif

  virtual void SetRoundedCorners(const RoundedCornersOptions& options);
  RoundedCornersOptions GetRoundedCorners() const;

  void SetClippingInsets(const ClippingInsetOptions& options);

  void ResetScaling();
  void SetScale(const ScaleAnimationOptions& options);
  float GetScaleX();
  float GetScaleY();
  void SetOpacity(const double opacity, const AnimationOptions& options);
  double GetOpacity();

  // Get children.
  int ChildCount() const { return static_cast<int>(children_.size()); }

  // Add/Remove children.
  void AddChildView(scoped_refptr<NativeView> view);
  bool RemoveChildView(NativeView* view);

  void RearrangeChildViews();

  // Get parent.
  NativeView* GetParent() const { return parent_; }

  // Get window.
  NativeWindow* GetWindow() const { return window_; }

  // Get the native View object.
  NATIVEVIEW GetNative() const { return view_; }

  // Internal: Set parent view.
  void SetParent(NativeView* parent);
  void BecomeContentView(NativeWindow* window);

  void SetWindow(NativeWindow* window);

  bool IsVibrant() const { return vibrant_; }
  bool IsBlurred() const { return blurred_; }

  void SetZIndex(int z_index);
  int GetZIndex() const;

  void SetClickThrough(bool click_through);
  bool IsClickThrough() const;
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  virtual void UpdateClickThrough();
  views::BoundsAnimator* GetOrCreateBoundsAnimator();
  void SetBlockScrollViewWhenFocus(bool block);
  bool IsBlockScrollViewWhenFocus() const;
  void UpdateBlockScrollViewWhenFocus();
#endif

  virtual void DetachChildView(NativeView* view);

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  // Should delete the |view_| in destructor.
  void set_delete_view(bool should) { delete_view_ = should; }
#endif

  void NotifyChildViewDetached(NativeView* view);

#if BUILDFLAG(IS_MAC)
  bool NotifyMouseDown(const NativeMouseEvent& event);
  bool NotifyMouseUp(const NativeMouseEvent& event);
  void NotifyMouseMove(const NativeMouseEvent& event);
  void NotifyMouseEnter(const NativeMouseEvent& event);
  void NotifyMouseLeave(const NativeMouseEvent& event);
  void NotifyCaptureLost();
  void NotifyWillStartLiveScroll(NativeView* view);
  void NotifyDidLiveScroll(NativeView* view);
  void NotifyDidEndLiveScroll(NativeView* view);
  void NotifyScrollWheel(NativeView* observed_view,
                         bool mouse_event,
                         float scrolling_delta_x,
                         float scrolling_delta_y,
                         std::string phase,
                         std::string momentum_phase);
#endif  // BUILDFLAG(IS_MAC)

  void NotifyDidScroll(NativeView* view);

  // Notify that view's size has changed.
  virtual void NotifySizeChanged(gfx::Size old_size, gfx::Size new_size);

  // Notify that native view is destroyed.
  void NotifyViewIsDeleting();

 protected:
  ~NativeView() override;

  void SetNativeView(NATIVEVIEW view);

  void InitView();
  void DestroyView();
  void SetVisibleImpl(bool visible);

  void AddChildViewImpl(NativeView* view);
  void RemoveChildViewImpl(NativeView* view);

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  // views::ViewObserver:
  void OnViewBoundsChanged(views::View* observed_view) override;
  void OnViewRemovedFromWidget(views::View* observed_view) override;
  void OnViewIsDeleting(views::View* observed_view) override;
  void OnViewHierarchyChanged(
      views::View* observed_view,
      const views::ViewHierarchyChangedDetails& details) override;
#endif

  virtual void SetWindowForChildren(NativeWindow* window);

 private:
  friend class base::RefCounted<NativeView>;

  base::ObserverList<Observer> observers_;

  // The view layer.
  std::vector<scoped_refptr<NativeView>> children_;

  // Relationships.
  NativeView* parent_ = nullptr;
  NativeWindow* window_ = nullptr;

  // The native implementation.
  NATIVEVIEW view_ = nullptr;

  bool vibrant_ = false;
  bool blurred_ = false;

  int z_index_ = 1;

  bool is_click_through_ = false;
  RoundedCornersOptions rounded_corners_;

#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  bool delete_view_ = true;
  gfx::Rect bounds_;
  std::unique_ptr<views::BoundsAnimator> bounds_animator_;
  bool block_scroll_view_when_focus = false;
#endif
};

}  // namespace electron

#endif  // SHELL_BROWSER_UI_NATIVE_VIEW_H_
