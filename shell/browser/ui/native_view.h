#ifndef SHELL_BROWSER_UI_NATIVE_VIEW_H_
#define SHELL_BROWSER_UI_NATIVE_VIEW_H_

#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "base/observer_list_types.h"
#include "base/supports_user_data.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
#include "ui/views/view_observer.h"
#endif

namespace gin_helper {
class Dictionary;
}

#if defined(OS_MAC)
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

#if defined(OS_MAC)
using NATIVEEVENT = NSEvent*;
using NATIVEVIEW = NSView*;
#elif defined(TOOLKIT_VIEWS)
using NATIVEVIEW = views::View*;
#endif

class NativeWindow;

#if defined(OS_MAC)
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
#endif  // defined(OS_MAC)

// The base class for all kinds of views.
class NativeView : public base::RefCounted<NativeView>,
                   public base::SupportsUserData
#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
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
#if defined(OS_MAC)
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
    virtual void OnDidScroll(NativeView* observed_view) {}
    virtual void OnWillStartLiveScroll(NativeView* observed_view) {}
    virtual void OnDidLiveScroll(NativeView* observed_view) {}
    virtual void OnDidEndLiveScroll(NativeView* observed_view) {}
#endif  // defined(OS_MAC)
    virtual void OnSizeChanged(NativeView* observed_view,
                               gfx::Size old_size,
                               gfx::Size new_size) {}

    virtual void OnViewIsDeleting(NativeView* observed_view) {}
  };

  NativeView();

  // Change position and size.
  virtual void SetBounds(const gfx::Rect& bounds,
                         const gin_helper::Dictionary& options);

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
  void SetBackgroundColor(SkColor color);

#if defined(OS_MAC)
  // Capture mouse.
  void SetCapture();
  void ReleaseCapture();
  bool HasCapture() const;

  void EnableMouseEvents();
  void SetMouseTrackingEnabled(bool enable);
  bool IsMouseTrackingEnabled();

  void SetWantsLayer(bool wants);
  bool WantsLayer() const;

  struct RoundedCornersOptions {
    float radius = 0.f;
    bool top_left = false;
    bool top_right = false;
    bool bottom_left = false;
    bool bottom_right = false;

    RoundedCornersOptions();
  };

  void SetRoundedCorners(const RoundedCornersOptions& options);

  struct ClippingInsetOptions {
    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;

    ClippingInsetOptions();
  };

  void SetClippingInsets(const ClippingInsetOptions& options);
#endif

  void ResetScaling();
  void SetScale(const gin_helper::Dictionary& options);
  float GetScaleX();
  float GetScaleY();
  void SetOpacity(const double opacity, const gin_helper::Dictionary& options);
  double GetOpacity();

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

  // Whether this class inherits from Container.
  virtual bool IsContainer() const;

  void SetZIndex(int z_index);
  int GetZIndex() const;

  void SetClickThrough(bool click_through);
  bool IsClickThrough() const;

  virtual void DetachChildView(NativeView* view);

  virtual void TriggerBeforeunloadEvents();

#if defined(OS_MAC)
  virtual void UpdateDraggableRegions();
#endif

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
  // Should delete the |view_| in destructor.
  void set_delete_view(bool should) { delete_view_ = should; }
#endif

  void NotifyChildViewDetached(NativeView* view);

#if defined(OS_MAC)
  bool NotifyMouseDown(const NativeMouseEvent& event);
  bool NotifyMouseUp(const NativeMouseEvent& event);
  void NotifyMouseMove(const NativeMouseEvent& event);
  void NotifyMouseEnter(const NativeMouseEvent& event);
  void NotifyMouseLeave(const NativeMouseEvent& event);
  void NotifyCaptureLost();
  void NotifyDidScroll(NativeView* view);
  void NotifyWillStartLiveScroll(NativeView* view);
  void NotifyDidLiveScroll(NativeView* view);
  void NotifyDidEndLiveScroll(NativeView* view);
#endif  // defined(OS_MAC)

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

#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
  // views::ViewObserver:
  void OnViewBoundsChanged(views::View* observed_view) override;
  void OnViewIsDeleting(views::View* observed_view) override;
#endif

  virtual void SetWindowForChildren(NativeWindow* window);

 private:
  friend class base::RefCounted<NativeView>;

  base::ObserverList<Observer> observers_;

  // Relationships.
  NativeView* parent_ = nullptr;
  NativeWindow* window_ = nullptr;

  // The native implementation.
  NATIVEVIEW view_ = nullptr;

  int z_index_ = 1;

  bool is_click_through_ = false;

#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
  bool delete_view_ = true;
  gfx::Rect bounds_;
#endif
};

}  // namespace electron

#endif  // SHELL_BROWSER_UI_NATIVE_VIEW_H_
