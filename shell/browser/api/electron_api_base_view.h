// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_
#define ELECTRON_SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_

#include <memory>
#include <string>
#include <vector>

#include "shell/common/gin_helper/error_thrower.h"
#include "shell/common/gin_helper/trackable_object.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/views/animation/bounds_animator.h"
#include "ui/views/view_observer.h"

#if BUILDFLAG(IS_MAC)
#ifdef __OBJC__
@class NSView;
#else
struct NSView;
#endif
#endif

namespace views {
class View;
}

namespace gin {
class Arguments;
template <typename T>
class Handle;
}  // namespace gin

namespace electron::api {

class BaseWindow;

class BaseView : public gin_helper::TrackableObject<BaseView>,
                 public views::ViewObserver {
 public:
  // Supported mouse event types.
  enum class MouseEventType {
    kUnknown,
    kDown,
    kUp,
    kMove,
    kEnter,
    kLeave,
  };

  struct RoundedCornersOptions {
    RoundedCornersOptions() = default;

    float radius = 0.f;
    bool top_left = false;
    bool top_right = false;
    bool bottom_left = false;
    bool bottom_right = false;
  };

  struct ClippingInsetOptions {
    ClippingInsetOptions() = default;

    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;
  };

  enum class TimingFunction {
    kLinear,
    kEaseIn,
    kEaseOut,
    kEaseInEaseOut,
    kDefault,
  };

  struct AnimationOptions {
    AnimationOptions() = default;

    bool animation = false;
    float duration = 0.4;
    TimingFunction timing_function = TimingFunction::kLinear;
    float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
    bool use_control_points = false;
  };

  struct BoundsAnimationOptions : public AnimationOptions {
    BoundsAnimationOptions() = default;

    gfx::Rect from_bounds;
    bool use_from_bounds = false;
  };

  enum class AnchorXPos {
    kUnknown,
    kLeft,
    kCenter,
    kRight,
    kPercentage,
  };

  enum class AnchorYPos {
    kUnknown,
    kTop,
    kCenter,
    kBottom,
    kPercentage,
  };

  struct ScaleAnimationOptions : public AnimationOptions {
    ScaleAnimationOptions() = default;

    float scale_x = 1.0, scale_y = 1.0;
    bool adjust_frame = true;
    AnchorXPos anchor_x_pos = AnchorXPos::kUnknown;
    AnchorYPos anchor_y_pos = AnchorYPos::kUnknown;
    float anchor_x_percentage = 0.0, anchor_y_percentage = 0.0;
  };

  static gin_helper::WrappableBase* New(gin_helper::ErrorThrower thrower,
                                        gin::Arguments* args);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  // disable copy
  BaseView(const BaseView&) = delete;
  BaseView& operator=(const BaseView&) = delete;

  bool IsView() const;
#if BUILDFLAG(IS_MAC)
  NSView* GetNSView() const { return nsview_; }
#endif
  views::View* GetView() const { return view_; }

  int32_t GetID() const;
  void EnsureDetachFromParent();
  bool IsClickThrough() const;

 protected:
  class CustomView;

  friend class BaseWindow;
  friend class CustomView;
  friend class ScrollView;

  BaseView();
  BaseView(gin::Arguments* args, bool vibrant, bool blurred);
  ~BaseView() override;

  // TrackableObject:
  void InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) override;

  // views::ViewObserver:
  void OnViewBoundsChanged(views::View* observed_view) override;
  void OnViewRemovedFromWidget(views::View* observed_view) override;
  void OnViewIsDeleting(views::View* observed_view) override;
  void OnViewHierarchyChanged(
      views::View* observed_view,
      const views::ViewHierarchyChangedDetails& details) override;

  // BaseView APIs.
  std::uintptr_t GetNativeID() const;
  void SetZIndex(int z_index);
  int GetZIndex() const;
  void SetClickThrough(bool click_through);
  virtual void SetBounds(const gfx::Rect& bounds, gin::Arguments* args);
  gfx::Rect GetBounds() const;
  gfx::Point OffsetFromView(gin::Handle<BaseView> from) const;
  gfx::Point OffsetFromWindow() const;
  void SetVisible(bool visible);
  bool IsVisible() const;
  bool IsTreeVisible() const;
  void Focus();
  bool HasFocus() const;
  void SetFocusable(bool focusable);
  bool IsFocusable() const;
  virtual void SetBackgroundColor(const std::string& color_name);
#if BUILDFLAG(IS_MAC)
  void SetVisualEffectMaterial(std::string material);
  std::string GetVisualEffectMaterial() const;
  void SetVisualEffectBlendingMode(std::string mode);
  std::string GetVisualEffectBlendingMode() const;
  void SetBlurTintColorWithSRGB(float r, float g, float b, float a);
  void SetBlurTintColorWithCalibratedWhite(float white, float alphaval);
  void SetBlurTintColorWithGenericGamma22White(float white, float alphaval);
  void SetBlurRadius(float radius);
  float GetBlurRadius() const;
  void SetBlurSaturationFactor(float factor);
  float GetBlurSaturationFactor() const;
  void SetCapture();
  void ReleaseCapture();
  bool HasCapture() const;
#endif  // BUILDFLAG(IS_MAC)
  void EnableMouseEvents();
  bool AreMouseEventsEnabled() const;
  void SetMouseTrackingEnabled(bool enable);
  bool IsMouseTrackingEnabled() const;
  virtual void SetRoundedCorners(const RoundedCornersOptions& options);
  void SetClippingInsets(const ClippingInsetOptions& options);
  void ResetScaling();
  void SetScale(const ScaleAnimationOptions& options);
  float GetScaleX() const;
  float GetScaleY() const;
  void SetOpacity(const double opacity, gin::Arguments* args);
  double GetOpacity() const;
  void AddChildView(gin::Handle<BaseView> base_view);
  virtual void RemoveChildView(gin::Handle<BaseView> base_view);
  void RearrangeChildViews();
  std::vector<v8::Local<v8::Value>> GetViews() const;
  std::vector<v8::Local<v8::Value>> GetRearrangedViews() const;
  std::vector<v8::Local<v8::Value>> GetNativelyRearrangedViews() const;
  v8::Local<v8::Value> GetParentView() const;
  v8::Local<v8::Value> GetParentWindow() const;

  // Helpers.

  bool IsVibrant() const { return vibrant_; }
  bool IsBlurred() const { return blurred_; }

  // Should delete the |view_| in destructor.
  void set_delete_view(bool should) { delete_view_ = should; }

  // Get children.
  int ChildCount() const { return static_cast<int>(api_children_.size()); }

  // Get parent.
  BaseView* GetParent() const { return parent_; }

  // Get window.
  BaseWindow* GetWindow() const { return window_; }

  void CreateView();

#if BUILDFLAG(IS_MAC)
  void SetView(NSView* view);
#endif
  void SetView(views::View* view);

  void DestroyView();

#if BUILDFLAG(IS_MAC)
  void SetWantsLayer(bool wants);
  bool WantsLayer() const;
#endif

  virtual void SetBackgroundColorImpl(const SkColor& color);

  void AddChildViewImpl(BaseView* view);
  void RemoveChildViewImpl(BaseView* view);

  virtual void ResetChildViews();

  void SetParent(BaseView* parent);
  void BecomeContentView(BaseWindow* window);

  virtual void SetWindow(BaseWindow* window);

  virtual void UpdateClickThrough();
  void SetBlockScrollViewWhenFocus(bool block);
  bool IsBlockScrollViewWhenFocus() const;
  void UpdateBlockScrollViewWhenFocus();

#if BUILDFLAG(IS_MAC)
  std::uintptr_t GetNativeIDMac() const;
  void SetClickThroughMac(bool click_through);
  bool IsClickThroughMac() const;
  void SetBoundsMac(const gfx::Rect& bounds, gin::Arguments* args);
  gfx::Rect GetBoundsMac() const;
  gfx::Point OffsetFromViewMac(gin::Handle<BaseView> from) const;
  gfx::Point OffsetFromWindowMac() const;
  void SetVisibleMac(bool visible);
  bool IsVisibleMac() const;
  bool IsTreeVisibleMac() const;
  void FocusMac();
  bool HasFocusMac() const;
  void SetFocusableMac(bool focusable);
  bool IsFocusableMac() const;
  void SetBackgroundColorMac(const std::string& color_name);
  void EnableMouseEventsMac();
  bool AreMouseEventsEnabledMac() const;
  void SetMouseTrackingEnabledMac(bool enable);
  bool IsMouseTrackingEnabledMac() const;
  void SetRoundedCornersMac(const RoundedCornersOptions& options);
  void SetClippingInsetsMac(const ClippingInsetOptions& options);
  void ResetScalingMac();
  void SetScaleMac(const ScaleAnimationOptions& options);
  float GetScaleXMac() const;
  float GetScaleYMac() const;
  void SetOpacityMac(const double opacity, gin::Arguments* args);
  double GetOpacityMac() const;
  void RearrangeChildViewsMac();
  std::vector<v8::Local<v8::Value>> GetNativelyRearrangedViewsMac() const;
  void CreateViewMac();
  void DestroyViewMac();
  void AddChildViewImplMac(BaseView* view);
  void RemoveChildViewImplMac(BaseView* view);
#endif  // BUILDFLAG(IS_MAC)

 public:
  // Notify that native view is destroyed.
  void NotifyViewIsDeleting();

  // Notify that view's size has changed.
  virtual void NotifySizeChanged(gfx::Size old_size, gfx::Size new_size);

  bool NotifyMouseEvent(const MouseEventType type,
                        const uint32_t timestamp,
                        const int button,
                        const gfx::Point& position_in_view,
                        const gfx::Point& position_in_window);

#if BUILDFLAG(IS_MAC)
  void NotifyCaptureLost();
#endif

 private:
#if BUILDFLAG(IS_MAC)
  NSView* nsview_ = nullptr;
#endif
  views::View* view_ = nullptr;

  bool vibrant_ = false;
  bool blurred_ = false;

  std::map<int32_t, v8::Global<v8::Value>> children_;
  std::vector<BaseView*> api_children_;

  int z_index_ = 1;
  bool is_click_through_ = false;
  RoundedCornersOptions rounded_corners_options_;

  bool delete_view_ = true;
  gfx::Rect bounds_;
  std::unique_ptr<views::BoundsAnimator> bounds_animator_;
  bool block_scroll_view_when_focus = false;
  bool mouse_events_enabled_ = false;
  bool mouse_tracking_enabled_ = false;

  // Relationships.
  BaseView* parent_ = nullptr;
  BaseWindow* window_ = nullptr;

  // Reference to JS wrapper to prevent garbage collection.
  v8::Global<v8::Value> self_ref_;
};

}  // namespace electron::api

namespace gin {

template <>
struct Converter<electron::api::BaseView::AnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::api::BaseView::AnimationOptions* options);
};

template <>
struct Converter<electron::api::BaseView::BoundsAnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::api::BaseView::BoundsAnimationOptions* options);
};

template <>
struct Converter<electron::api::BaseView::ScaleAnimationOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::api::BaseView::ScaleAnimationOptions* options);
};

}  // namespace gin

#endif  // ELECTRON_SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_
