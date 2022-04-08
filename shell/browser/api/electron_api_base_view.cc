#include "shell/browser/api/electron_api_base_view.h"

#include "gin/handle.h"
#include "shell/browser/browser.h"
#include "shell/browser/native_window.h"
#include "shell/common/color_util.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_converters/value_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"

#if defined(TOOLKIT_VIEWS) && !defined(OS_MAC)
#include "ui/views/view.h"
#endif

#if defined(OS_MAC)
namespace gin {

template <>
struct Converter<electron::NativeView::RoundedCornersOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::NativeView::RoundedCornersOptions* options) {
    gin_helper::Dictionary params;
    if (!ConvertFromV8(isolate, val, &params))
      return false;

    *options = electron::NativeView::RoundedCornersOptions();

    float radius;
    if (params.Get("radius", &radius))
      options->radius = radius;
    bool top_left = false;
    if (params.Get("topLeft", &top_left) && top_left)
      options->top_left = top_left;
    bool top_right = false;
    if (params.Get("topRight", &top_right) && top_right)
      options->top_right = top_right;
    bool bottom_left = false;
    if (params.Get("bottomLeft", &bottom_left) && bottom_left)
      options->bottom_left = bottom_left;
    bool bottom_right = false;
    if (params.Get("bottomRight", &bottom_right) && bottom_right)
      options->bottom_right = bottom_right;

    return true;
  }
};

template <>
struct Converter<electron::NativeView::ClippingInsetOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::NativeView::ClippingInsetOptions* options) {
    gin_helper::Dictionary params;
    if (!ConvertFromV8(isolate, val, &params))
      return false;

    *options = electron::NativeView::ClippingInsetOptions();

    int top = 0;
    if (params.Get("top", &top) && top)
      options->top = top;
    int left = 0;
    if (params.Get("left", &left) && left)
      options->left = left;
    int bottom = 0;
    if (params.Get("bottom", &bottom) && bottom)
      options->bottom = bottom;
    int right = 0;
    if (params.Get("right", &right) && right)
      options->right = right;

    return true;
  }
};

}  // namespace gin
#endif

namespace electron {

namespace api {

namespace {

#if defined(OS_MAC)
std::string ConvertFromEventType(EventType type) {
  if (type == EventType::kLeftMouseDown)
    return "left-mouse-down";
  else if (type == EventType::kRightMouseDown)
    return "right-mouse-down";
  else if (type == EventType::kOtherMouseDown)
    return "other-mouse-down";
  else if (type == EventType::kLeftMouseUp)
    return "left-mouse-up";
  else if (type == EventType::kRightMouseUp)
    return "right-mouse-up";
  else if (type == EventType::kOtherMouseUp)
    return "other-mouse-up";
  else if (type == EventType::kMouseMove)
    return "mouse-move";
  else if (type == EventType::kMouseEnter)
    return "mouse-enter";
  else if (type == EventType::kMouseLeave)
    return "mouse-leave";
  return "unknown";
}
#endif  // defined(OS_MAC)

}  // namespace

BaseView::BaseView(v8::Isolate* isolate, NativeView* native_view)
    : view_(native_view) {
  view_->AddObserver(this);
}

BaseView::BaseView(gin::Arguments* args, NativeView* native_view)
    : BaseView(args->isolate(), native_view) {
  InitWithArgs(args);
}

BaseView::~BaseView() {
  // Remove global reference so the JS object can be garbage collected.
  self_ref_.Reset();
}

void BaseView::InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
  AttachAsUserData(view_.get());
  gin_helper::TrackableObject<BaseView>::InitWith(isolate, wrapper);

  // Reference this object in case it got garbage collected.
  self_ref_.Reset(isolate, wrapper);
}

void BaseView::OnChildViewDetached(NativeView* observed_view,
                                   NativeView* view) {
  auto* api_view = TrackableObject::FromWrappedClass(isolate(), view);
  if (api_view)
    ResetChildView(api_view);
}

#if defined(OS_MAC)
bool BaseView::OnMouseDown(NativeView* observed_view,
                           const NativeMouseEvent& event) {
  return Emit("mouse-down", ConvertFromEventType(event.type), event.timestamp,
              event.button, event.position_in_view, event.position_in_window);
}

bool BaseView::OnMouseUp(NativeView* observed_view,
                         const NativeMouseEvent& event) {
  return Emit("mouse-up", ConvertFromEventType(event.type), event.timestamp,
              event.button, event.position_in_view, event.position_in_window);
}

void BaseView::OnMouseMove(NativeView* observed_view,
                           const NativeMouseEvent& event) {
  Emit("mouse-move", ConvertFromEventType(event.type), event.timestamp,
       event.button, event.position_in_view, event.position_in_window);
}

void BaseView::OnMouseEnter(NativeView* observed_view,
                            const NativeMouseEvent& event) {
  Emit("mouse-enter", ConvertFromEventType(event.type), event.timestamp,
       event.button, event.position_in_view, event.position_in_window);
}

void BaseView::OnMouseLeave(NativeView* observed_view,
                            const NativeMouseEvent& event) {
  Emit("mouse-leave", ConvertFromEventType(event.type), event.timestamp,
       event.button, event.position_in_view, event.position_in_window);
}

void BaseView::OnCaptureLost(NativeView* observed_view) {
  Emit("capture-lost");
}
#endif  // defined(OS_MAC)

void BaseView::OnSizeChanged(NativeView* observed_view,
                             gfx::Size old_size,
                             gfx::Size new_size) {
  Emit("size-changed", old_size, new_size);
}

void BaseView::OnViewIsDeleting(NativeView* observed_view) {
  RemoveFromWeakMap();
  view_->RemoveObserver(this);

  // We can not call Destroy here because we need to call Emit first, but we
  // also do not want any method to be used, so just mark as destroyed here.
  MarkDestroyed();

  EnsureDetachFromParent();
  ResetChildViews();

  // Destroy the native class when window is closed.
  base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, GetDestroyClosure());
}

bool BaseView::IsContainer() const {
  return view_->IsContainer();
}

void BaseView::SetZIndex(int z_index) {
  view_->SetZIndex(z_index);
}

int BaseView::GetZIndex() const {
  return view_->GetZIndex();
}

#if defined(OS_MAC)
void BaseView::SetClickThrough(bool clickThrough) {
  view_->SetClickThrough(clickThrough);
}

bool BaseView::IsClickThrough() const {
  return view_->IsClickThrough();
}
#endif

void BaseView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  gin::Dictionary options = gin::Dictionary::CreateEmpty(args->isolate());
  args->GetNext(&options);
  view_->SetBounds(bounds, options);
}

gfx::Rect BaseView::GetBounds() const {
  return view_->GetBounds();
}

gfx::Point BaseView::OffsetFromView(gin::Handle<BaseView> from) const {
  return view_->OffsetFromView(from->view());
}

gfx::Point BaseView::OffsetFromWindow() const {
  return view_->OffsetFromWindow();
}

void BaseView::SetVisible(bool visible) {
  view_->SetVisible(visible);
}

bool BaseView::IsVisible() const {
  return view_->IsVisible();
}

bool BaseView::IsTreeVisible() const {
  return view_->IsTreeVisible();
}

void BaseView::Focus() {
  view_->Focus();
}

bool BaseView::HasFocus() const {
  return view_->HasFocus();
}

void BaseView::SetFocusable(bool focusable) {
  view_->SetFocusable(focusable);
}

bool BaseView::IsFocusable() const {
  return view_->IsFocusable();
}

void BaseView::SetBackgroundColor(const std::string& color_name) {
  view_->SetBackgroundColor(ParseHexColor(color_name));
}

#if defined(OS_MAC)
void BaseView::SetCapture() {
  view_->SetCapture();
}

void BaseView::ReleaseCapture() {
  view_->ReleaseCapture();
}

bool BaseView::HasCapture() const {
  return view_->HasCapture();
}

void BaseView::EnableMouseEvents() {
  view_->EnableMouseEvents();
}

void BaseView::SetMouseTrackingEnabled(bool enable) {
  view_->SetMouseTrackingEnabled(enable);
}

bool BaseView::IsMouseTrackingEnabled() {
  return view_->IsMouseTrackingEnabled();
}

void BaseView::SetRoundedCorners(
    const NativeView::RoundedCornersOptions& options) {
  return view_->SetRoundedCorners(options);
}

void BaseView::SetClippingInsets(
    const NativeView::ClippingInsetOptions& options) {
  return view_->SetClippingInsets(options);
}
#endif

void BaseView::ResetScaling() {
  view_->ResetScaling();
}

void BaseView::SetScale(const gin_helper::Dictionary& options) {
  view_->SetScale(options);
}

float BaseView::GetScaleX() {
  return view_->GetScaleX();
}

float BaseView::GetScaleY() {
  return view_->GetScaleY();
}

void BaseView::SetOpacity(const double opacity, gin::Arguments* args) {
  gin::Dictionary options = gin::Dictionary::CreateEmpty(args->isolate());
  args->GetNext(&options);
  view_->SetOpacity(opacity, options);
}

double BaseView::GetOpacity() {
  return view_->GetOpacity();
}

int32_t BaseView::GetID() const {
  return weak_map_id();
}

v8::Local<v8::Value> BaseView::GetParentView() const {
  NativeView* parent_view = view_->GetParent();
  if (parent_view) {
    auto* existing_view =
        TrackableObject::FromWrappedClass(isolate(), parent_view);
    if (existing_view)
      return existing_view->GetWrapper();
  }
  return v8::Null(isolate());
}

v8::Local<v8::Value> BaseView::GetParentWindow() const {
  NativeWindow* parent_window = view_->GetWindow();
  if (!view_->GetParent() && parent_window) {
    auto* existing_window =
        TrackableObject::FromWrappedClass(isolate(), parent_window);
    if (existing_window)
      return existing_window->GetWrapper();
  }
  return v8::Null(isolate());
}

bool BaseView::EnsureDetachFromParent() {
  auto* owner_view = view()->GetParent();
  if (owner_view) {
    owner_view->DetachChildView(view());
  } else {
    auto* owner_window = view()->GetWindow();
    if (owner_window)
      return owner_window->DetachChildView(view());
  }
  return true;
}

void BaseView::ResetChildView(BaseView* view) {}

void BaseView::ResetChildViews() {}

// static
gin_helper::WrappableBase* BaseView::New(gin_helper::ErrorThrower thrower,
                                         gin::Arguments* args) {
  if (!Browser::Get()->is_ready()) {
    thrower.ThrowError("Cannot create View before app is ready");
    return nullptr;
  }

  return new BaseView(args, new NativeView());
}

// static
void BaseView::BuildPrototype(v8::Isolate* isolate,
                              v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "BaseView"));
  gin_helper::Destroyable::MakeDestroyable(isolate, prototype);
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetProperty("id", &BaseView::GetID)
      .SetProperty("isContainer", &BaseView::IsContainer)
      .SetProperty("zIndex", &BaseView::GetZIndex, &BaseView::SetZIndex)
#if defined(OS_MAC)
      .SetProperty("clickThrough", &BaseView::IsClickThrough,
                   &BaseView::SetClickThrough)
#endif
      .SetMethod("setBounds", &BaseView::SetBounds)
      .SetMethod("getBounds", &BaseView::GetBounds)
      .SetMethod("offsetFromView", &BaseView::OffsetFromView)
      .SetMethod("offsetFromWindow", &BaseView::OffsetFromWindow)
      .SetMethod("setVisible", &BaseView::SetVisible)
      .SetMethod("isVisible", &BaseView::IsVisible)
      .SetMethod("isTreeVisible", &BaseView::IsTreeVisible)
      .SetMethod("focus", &BaseView::Focus)
      .SetMethod("hasFocus", &BaseView::HasFocus)
      .SetMethod("setFocusable", &BaseView::SetFocusable)
      .SetMethod("isFocusable", &BaseView::IsFocusable)
      .SetMethod("setBackgroundColor", &BaseView::SetBackgroundColor)
#if defined(OS_MAC)
      .SetMethod("setCapture", &BaseView::SetCapture)
      .SetMethod("releaseCapture", &BaseView::ReleaseCapture)
      .SetMethod("hasCapture", &BaseView::HasCapture)
      .SetMethod("enableMouseEvents", &BaseView::EnableMouseEvents)
      .SetMethod("setMouseTrackingEnabled", &BaseView::SetMouseTrackingEnabled)
      .SetMethod("isMouseTrackingEnabled", &BaseView::IsMouseTrackingEnabled)
      .SetMethod("setRoundedCorners", &BaseView::SetRoundedCorners)
      .SetMethod("setClippingInsets", &BaseView::SetClippingInsets)
#endif
      .SetMethod("resetScaling", &BaseView::ResetScaling)
      .SetMethod("setScale", &BaseView::SetScale)
      .SetMethod("getScaleX", &BaseView::GetScaleX)
      .SetMethod("getScaleY", &BaseView::GetScaleY)
      .SetMethod("setOpacity", &BaseView::SetOpacity)
      .SetMethod("getOpacity", &BaseView::GetOpacity)
      .SetMethod("getParentView", &BaseView::GetParentView)
      .SetMethod("getParentWindow", &BaseView::GetParentWindow)
      .Build();
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::BaseView;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  BaseView::SetConstructor(isolate, base::BindRepeating(&BaseView::New));

  gin_helper::Dictionary constructor(
      isolate,
      BaseView::GetConstructor(isolate)->GetFunction(context).ToLocalChecked());
  constructor.SetMethod("fromId", &BaseView::FromWeakMapID);
  constructor.SetMethod("getAllViews", &BaseView::GetAll);

  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("BaseView", constructor);
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_base_view, Initialize)
