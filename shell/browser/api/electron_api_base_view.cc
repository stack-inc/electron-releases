// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_base_view.h"

#include <algorithm>
#include <limits>
#include <string>
#include <utility>

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "gin/handle.h"
#include "shell/browser/api/electron_api_base_window.h"
#include "shell/browser/browser.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_converters/value_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"

namespace gin {

namespace {

bool ParseAnchorPercentage(std::string anchor_str, int* percentage) {
  if (anchor_str.empty())
    return false;
  if (!base::StringToInt(anchor_str, percentage))
    return false;
  if (*percentage > 100)
    *percentage = 100;
  else if (*percentage < 0)
    *percentage = 0;
  return true;
}

void ConvertAnimationOptions(
    const gin_helper::Dictionary& params,
    electron::api::BaseView::AnimationOptions* options) {
  float duration = 1.0;
  if (params.Get("duration", &duration)) {
    options->duration = duration;
    options->animation = true;
  }

  std::string tfunction_name;
  if (params.Get("timingFunction", &tfunction_name)) {
    tfunction_name = base::ToLowerASCII(tfunction_name);
    base::TrimWhitespaceASCII(tfunction_name, base::TRIM_ALL, &tfunction_name);
    if (tfunction_name == "linear")
      options->timing_function =
          electron::api::BaseView::TimingFunction::kLinear;
    else if (tfunction_name == "easein")
      options->timing_function =
          electron::api::BaseView::TimingFunction::kEaseIn;
    else if (tfunction_name == "easeout")
      options->timing_function =
          electron::api::BaseView::TimingFunction::kEaseOut;
    else if (tfunction_name == "easeineaseout")
      options->timing_function =
          electron::api::BaseView::TimingFunction::kEaseInEaseOut;
    else if (tfunction_name == "default")
      options->timing_function =
          electron::api::BaseView::TimingFunction::kDefault;
    options->animation = true;
  }

  float cx1 = 0.0, cy1 = 0.0, cx2 = 1.0, cy2 = 1.0;
  gin_helper::Dictionary timing_control_points;
  if (params.Get("timingControlPoints", &timing_control_points)) {
    if (timing_control_points.Get("x1", &cx1)) {
      options->cx1 = cx1;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("y1", &cy1)) {
      options->cy1 = cy1;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("x2", &cx2)) {
      options->cx2 = cx2;
      options->use_control_points = true;
    }
    if (timing_control_points.Get("y2", &cy2)) {
      options->cy2 = cy2;
      options->use_control_points = true;
    }
    options->animation = true;
  }
}

}  // namespace

template <>
struct Converter<electron::api::BaseView::RoundedCornersOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::api::BaseView::RoundedCornersOptions* options) {
    gin_helper::Dictionary params;
    if (!ConvertFromV8(isolate, val, &params))
      return false;

    *options = electron::api::BaseView::RoundedCornersOptions();

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
struct Converter<electron::api::BaseView::ClippingInsetOptions> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::api::BaseView::ClippingInsetOptions* options) {
    gin_helper::Dictionary params;
    if (!ConvertFromV8(isolate, val, &params))
      return false;

    *options = electron::api::BaseView::ClippingInsetOptions();

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

bool Converter<electron::api::BaseView::AnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::api::BaseView::AnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::api::BaseView::AnimationOptions();

  ConvertAnimationOptions(params, options);

  return true;
}

bool Converter<electron::api::BaseView::BoundsAnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::api::BaseView::BoundsAnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::api::BaseView::BoundsAnimationOptions();

  ConvertAnimationOptions(params, options);

  gfx::Rect from_bounds;
  if (params.Get("fromBounds", &from_bounds)) {
    options->from_bounds = from_bounds;
    options->use_from_bounds = true;
  }

  return true;
}

bool Converter<electron::api::BaseView::ScaleAnimationOptions>::FromV8(
    v8::Isolate* isolate,
    v8::Local<v8::Value> val,
    electron::api::BaseView::ScaleAnimationOptions* options) {
  gin_helper::Dictionary params;
  if (!ConvertFromV8(isolate, val, &params))
    return false;

  *options = electron::api::BaseView::ScaleAnimationOptions();

  gin_helper::Dictionary animation_params;
  if (params.Get("animation", &animation_params))
    ConvertAnimationOptions(animation_params, options);

  float scale_x = 1.0, scale_y = 1.0;
  if (params.Get("scaleX", &scale_x))
    options->scale_x = scale_x;
  if (params.Get("scaleY", &scale_y))
    options->scale_y = scale_y;
  bool adjust_frame = true;
  if (params.Get("adjustFrame", &adjust_frame))
    options->adjust_frame = adjust_frame;

  int percentage;
  std::string anchor_x;
  if (params.Get("anchorX", &anchor_x)) {
    anchor_x = base::ToLowerASCII(anchor_x);
    base::TrimWhitespaceASCII(anchor_x, base::TRIM_ALL, &anchor_x);
    if (anchor_x == "left") {
      options->anchor_x_pos = electron::api::BaseView::AnchorXPos::kLeft;
    } else if (anchor_x == "center") {
      options->anchor_x_pos = electron::api::BaseView::AnchorXPos::kCenter;
    } else if (anchor_x == "right") {
      options->anchor_x_pos = electron::api::BaseView::AnchorXPos::kRight;
    } else if (ParseAnchorPercentage(anchor_x, &percentage)) {
      options->anchor_x_percentage = static_cast<float>(percentage);
      options->anchor_x_pos = electron::api::BaseView::AnchorXPos::kPercentage;
    }
  }

  std::string anchor_y;
  if (params.Get("anchorY", &anchor_y)) {
    anchor_y = base::ToLowerASCII(anchor_y);
    base::TrimWhitespaceASCII(anchor_y, base::TRIM_ALL, &anchor_y);
    if (anchor_y == "top") {
      options->anchor_y_pos = electron::api::BaseView::AnchorYPos::kTop;
    } else if (anchor_y == "center") {
      options->anchor_y_pos = electron::api::BaseView::AnchorYPos::kCenter;
    } else if (anchor_y == "bottom") {
      options->anchor_y_pos = electron::api::BaseView::AnchorYPos::kBottom;
    } else if (ParseAnchorPercentage(anchor_y, &percentage)) {
      options->anchor_y_percentage = static_cast<float>(percentage);
      options->anchor_y_pos = electron::api::BaseView::AnchorYPos::kPercentage;
    }
  }

  return true;
}

}  // namespace gin

namespace electron {

namespace api {

BaseView::BaseView() = default;

BaseView::BaseView(gin::Arguments* args, bool vibrant, bool blurred)
    : vibrant_(vibrant), blurred_(blurred) {
  CreateView();
  InitWithArgs(args);
}

BaseView::~BaseView() {
  DestroyView();
  // Remove global reference so the JS object can be garbage collected.
  self_ref_.Reset();
}

void BaseView::InitWith(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
  gin_helper::TrackableObject<BaseView>::InitWith(isolate, wrapper);

  // Reference this object in case it got garbage collected.
  self_ref_.Reset(isolate, wrapper);
}

int32_t BaseView::GetID() const {
  return weak_map_id();
}

bool BaseView::EnsureDetachFromParent() {
  if (GetParent()) {
    GetParent()->RemoveChildView(gin::CreateHandle(isolate(), this));
  } else if (GetWindow()) {
    return GetWindow()->RemoveChildView(gin::CreateHandle(isolate(), this));
  }
  return true;
}

void BaseView::SetZIndex(int z_index) {
  z_index_ = z_index;
}

int BaseView::GetZIndex() const {
  return z_index_;
}

void BaseView::AddChildView(gin::Handle<BaseView> base_view) {
  if (!IsView())
    return;

  auto iter = children_.find(base_view->GetID());
  if (iter == children_.end()) {
    if (!base_view->EnsureDetachFromParent())
      return;
    if (base_view.get() == this || base_view->GetParent())
      return;
    AddChildViewImpl(base_view.get());
    base_view->SetParent(this);
    children_[base_view->GetID()].Reset(isolate(), base_view.ToV8());
    api_children_.insert(api_children_.begin() + ChildCount(), base_view.get());
    RearrangeChildViews();
  }
}

void BaseView::RemoveChildView(gin::Handle<BaseView> base_view) {
  if (!IsView())
    return;

  auto iter = children_.find(base_view->GetID());
  if (iter != children_.end()) {
    RemoveChildViewImpl(base_view.get());
    base_view->SetParent(nullptr);
    iter->second.Reset();
    children_.erase(iter);
    const auto api_iter(
        std::find(api_children_.begin(), api_children_.end(), base_view.get()));
    if (api_iter != api_children_.end())
      api_children_.erase(api_iter);
  }
}

std::vector<v8::Local<v8::Value>> BaseView::GetViews() const {
  std::vector<v8::Local<v8::Value>> ret;

  for (auto const& child_iter : children_) {
    if (!child_iter.second.IsEmpty())
      ret.push_back(v8::Local<v8::Value>::New(isolate(), child_iter.second));
  }

  return ret;
}

v8::Local<v8::Value> BaseView::GetParentView() const {
  if (GetParent())
    return GetParent()->GetWrapper();
  return v8::Null(isolate());
}

v8::Local<v8::Value> BaseView::GetParentWindow() const {
  if (!GetParent() && GetWindow()) {
    return GetWindow()->GetWrapper();
  }
  return v8::Null(isolate());
}

void BaseView::SetBackgroundColorImpl(const SkColor& color) {}

void BaseView::ResetChildViews() {
  v8::HandleScope scope(isolate());

  for (auto& item : children_) {
    gin::Handle<BaseView> base_view;
    if (gin::ConvertFromV8(isolate(),
                           v8::Local<v8::Value>::New(isolate(), item.second),
                           &base_view) &&
        !base_view.IsEmpty()) {
      // There's a chance that the BaseView may have been reparented - only
      // reset if the owner view is *this* view.
      auto* parent_view = base_view->GetParent();
      if (parent_view && parent_view == this)
        base_view->SetParent(nullptr);
    }

    item.second.Reset();
  }

  children_.clear();
  api_children_.clear();
}

void BaseView::SetParent(BaseView* parent) {
  if (parent) {
    SetWindow(parent->window_);
  } else {
    SetWindow(nullptr);
  }
  parent_ = parent;
}

void BaseView::BecomeContentView(BaseWindow* window) {
  SetWindow(window);
  parent_ = nullptr;
}

void BaseView::SetWindow(BaseWindow* window) {
  window_ = window;
  SetWindowForChildren(window);
}

void BaseView::SetWindowForChildren(BaseWindow* window) {
  for (BaseView* child : api_children_)
    child->SetWindow(window);
}

void BaseView::NotifySizeChanged(gfx::Size old_size, gfx::Size new_size) {
  Emit("size-changed", old_size, new_size);
}

void BaseView::NotifyViewIsDeleting() {
  RemoveFromWeakMap();

  // We can not call Destroy here because we need to call Emit first, but we
  // also do not want any method to be used, so just mark as destroyed here.
  MarkDestroyed();

  EnsureDetachFromParent();
  ResetChildViews();

  // Destroy the native class when window is closed.
  base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, GetDestroyClosure());
}

// static
gin_helper::WrappableBase* BaseView::New(gin_helper::ErrorThrower thrower,
                                         gin::Arguments* args) {
  if (!Browser::Get()->is_ready()) {
    thrower.ThrowError("Cannot create View before app is ready");
    return nullptr;
  }

  gin::Dictionary options = gin::Dictionary::CreateEmpty(args->isolate());
  args->GetNext(&options);
  bool vibrant = false;
  options.Get("vibrant", &vibrant);
  bool blurred = false;
  options.Get("blurred", &blurred);

  return new BaseView(args, vibrant, blurred);
}

// static
void BaseView::BuildPrototype(v8::Isolate* isolate,
                              v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "BaseView"));
  gin_helper::Destroyable::MakeDestroyable(isolate, prototype);
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetProperty("id", &BaseView::GetID)
      .SetProperty("zIndex", &BaseView::GetZIndex, &BaseView::SetZIndex)
      .SetProperty("clickThrough", &BaseView::IsClickThrough,
                   &BaseView::SetClickThrough)
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
#if BUILDFLAG(IS_MAC)
      .SetMethod("setVisualEffectMaterial", &BaseView::SetVisualEffectMaterial)
      .SetMethod("getVisualEffectMaterial", &BaseView::GetVisualEffectMaterial)
      .SetMethod("setVisualEffectBlendingMode",
                 &BaseView::SetVisualEffectBlendingMode)
      .SetMethod("getVisualEffectBlendingMode",
                 &BaseView::GetVisualEffectBlendingMode)
      .SetMethod("setBlurTintColorWithSRGB",
                 &BaseView::SetBlurTintColorWithSRGB)
      .SetMethod("setBlurTintColorWithCalibratedWhite",
                 &BaseView::SetBlurTintColorWithCalibratedWhite)
      .SetMethod("setBlurTintColorWithGenericGamma22White",
                 &BaseView::SetBlurTintColorWithGenericGamma22White)
      .SetMethod("setBlurRadius", &BaseView::SetBlurRadius)
      .SetMethod("getBlurRadius", &BaseView::GetBlurRadius)
      .SetMethod("setBlurSaturationFactor", &BaseView::SetBlurSaturationFactor)
      .SetMethod("getBlurSaturationFactor", &BaseView::GetBlurSaturationFactor)
      .SetMethod("setCapture", &BaseView::SetCapture)
      .SetMethod("releaseCapture", &BaseView::ReleaseCapture)
      .SetMethod("hasCapture", &BaseView::HasCapture)
      .SetMethod("enableMouseEvents", &BaseView::EnableMouseEvents)
      .SetMethod("setMouseTrackingEnabled", &BaseView::SetMouseTrackingEnabled)
      .SetMethod("isMouseTrackingEnabled", &BaseView::IsMouseTrackingEnabled)
#endif
      .SetMethod("setRoundedCorners", &BaseView::SetRoundedCorners)
      .SetMethod("setClippingInsets", &BaseView::SetClippingInsets)
      .SetMethod("resetScaling", &BaseView::ResetScaling)
      .SetMethod("setScale", &BaseView::SetScale)
      .SetMethod("getScaleX", &BaseView::GetScaleX)
      .SetMethod("getScaleY", &BaseView::GetScaleY)
      .SetMethod("setOpacity", &BaseView::SetOpacity)
      .SetMethod("getOpacity", &BaseView::GetOpacity)
      .SetMethod("addChildView", &BaseView::AddChildView)
      .SetMethod("removeChildView", &BaseView::RemoveChildView)
      .SetMethod("rearrangeChildViews", &BaseView::RearrangeChildViews)
      .SetMethod("getViews", &BaseView::GetViews)
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
