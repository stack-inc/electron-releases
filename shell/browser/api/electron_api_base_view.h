#ifndef SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_
#define SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_

#include <memory>
#include <string>

#include "shell/browser/ui/native_view.h"
#include "shell/common/gin_helper/error_thrower.h"
#include "shell/common/gin_helper/trackable_object.h"

namespace gin {
class Arguments;
template <typename T>
class Handle;
}  // namespace gin

namespace electron {

namespace api {

class BaseView : public gin_helper::TrackableObject<BaseView>,
                 public NativeView::Observer {
 public:
  static gin_helper::WrappableBase* New(gin_helper::ErrorThrower thrower,
                                        gin::Arguments* args);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  NativeView* view() const { return view_.get(); }

  int32_t GetID() const;

  bool EnsureDetachFromParent();

  // disable copy
  BaseView(const BaseView&) = delete;
  BaseView& operator=(const BaseView&) = delete;

 protected:
  friend class ScrollView;

  // Common constructor.
  BaseView(v8::Isolate* isolate, NativeView* native_view);
  // Creating independent BaseView instance.
  BaseView(gin::Arguments* args, NativeView* native_view);
  ~BaseView() override;

  // TrackableObject:
  void InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) override;

  // NativeView::Observer:
  void OnChildViewDetached(NativeView* observed_view,
                           NativeView* view) override;
#if defined(OS_MAC)
  bool OnMouseDown(NativeView* observed_view,
                   const NativeMouseEvent& event) override;
  bool OnMouseUp(NativeView* observed_view,
                 const NativeMouseEvent& event) override;
  void OnMouseMove(NativeView* observed_view,
                   const NativeMouseEvent& event) override;
  void OnMouseEnter(NativeView* observed_view,
                    const NativeMouseEvent& event) override;
  void OnMouseLeave(NativeView* observed_view,
                    const NativeMouseEvent& event) override;
  void OnCaptureLost(NativeView* observed_view) override;
#endif  // defined(OS_MAC)
  void OnSizeChanged(NativeView* observed_view,
                     gfx::Size old_size,
                     gfx::Size new_size) override;
  void OnViewIsDeleting(NativeView* observed_view) override;

  bool IsContainer() const;
  void SetZIndex(int z_index);
  int GetZIndex() const;
#if defined(OS_MAC)
  void SetClickThrough(bool clickThrough);
  bool IsClickThrough() const;
#endif
  void SetBounds(const gfx::Rect& bounds, gin::Arguments* args);
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
  void SetBackgroundColor(const std::string& color_name);
#if defined(OS_MAC)
  void SetCapture();
  void ReleaseCapture();
  bool HasCapture() const;
  void EnableMouseEvents();
  void SetMouseTrackingEnabled(bool enable);
  bool IsMouseTrackingEnabled();
  void SetRoundedCorners(const NativeView::RoundedCornersOptions& options);
  void SetClippingInsets(const NativeView::ClippingInsetOptions& options);
#endif
  void ResetScaling();
  void SetScale(const gin_helper::Dictionary& options);
  float GetScaleX();
  float GetScaleY();
  void SetOpacity(const double opacity, gin::Arguments* args);
  double GetOpacity();
  v8::Local<v8::Value> GetParentView() const;
  v8::Local<v8::Value> GetParentWindow() const;

  virtual void ResetChildView(BaseView* view);
  virtual void ResetChildViews();

 private:
  scoped_refptr<NativeView> view_;

  // Reference to JS wrapper to prevent garbage collection.
  v8::Global<v8::Value> self_ref_;
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_BROWSER_API_ELECTRON_API_BASE_VIEW_H_
