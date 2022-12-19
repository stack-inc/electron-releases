// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_scroll_view.h"

#include "gin/handle.h"
#include "shell/browser/browser.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"
#include "ui/base/ui_base_features.h"

namespace electron {

namespace api {

ScrollView::ScrollView(gin::Arguments* args,
                       const gin_helper::Dictionary& options) {
  CreateScrollView();

#if !BUILDFLAG(IS_MAC)
  bool smooth_scroll = false;
  if (options.Get("smoothScroll", &smooth_scroll))
    SetSmoothScroll(smooth_scroll);
  bool block_scroll_view_when_focus = false;
  if (options.Get("blockScrollViewWhenFocus", &block_scroll_view_when_focus))
    SetBlockScrollViewWhenFocus(block_scroll_view_when_focus);
#endif  // !BUILDFLAG(IS_MAC)

  InitWithArgs(args);
}

ScrollView::~ScrollView() = default;

void ScrollView::RemoveChildView(gin::Handle<BaseView> base_view) {
  if (base_view.get() == api_content_view_) {
    ResetCurrentContentViewImpl();
    api_content_view_->SetParent(nullptr);
    api_content_view_ = nullptr;
    content_view_.Reset();
    return;
  }

  BaseView::RemoveChildView(base_view);
}

void ScrollView::ResetChildViews() {
  BaseView::ResetChildViews();

  if (api_content_view_)
    api_content_view_->SetParent(nullptr);
  content_view_.Reset();
  api_content_view_ = nullptr;
}

void ScrollView::SetWindowForChildren(BaseWindow* window) {
  BaseView::SetWindowForChildren(window);

  if (api_content_view_)
    api_content_view_->SetWindow(window);
}

void ScrollView::SetContentView(gin::Handle<BaseView> base_view) {
  if (base_view.IsEmpty() || base_view.get() == this ||
      base_view.get() == api_content_view_)
    return;
  base_view->EnsureDetachFromParent();
  if (base_view->GetParent() || base_view->GetWindow())
    return;
  if (api_content_view_)
    api_content_view_->SetParent(nullptr);
  SetContentViewImpl(base_view.get());
  base_view->SetParent(this);
  content_view_.Reset(isolate(), base_view.ToV8());
  api_content_view_ = base_view.get();
}

v8::Local<v8::Value> ScrollView::GetContentView() const {
  if (content_view_.IsEmpty())
    return v8::Null(isolate());

  return v8::Local<v8::Value>::New(isolate(), content_view_);
}

gfx::Size ScrollView::GetContentSize() const {
  return api_content_view_->GetBounds().size();
}

v8::Local<v8::Promise> ScrollView::SetScrollPosition(gfx::Point point) {
  gin_helper::Promise<void> promise(isolate());
  auto handle = promise.GetHandle();
  SetScrollPositionImpl(
      point, base::BindOnce(
                 [](gin_helper::Promise<void> promise, std::string error) {
                   if (error.empty()) {
                     promise.Resolve();
                   } else {
                     promise.RejectWithErrorMessage(error);
                   }
                 },
                 std::move(promise)));
  return handle;
}

void ScrollView::NotifyDidScroll() {
  Emit("did-scroll");
}

// static
gin_helper::WrappableBase* ScrollView::New(gin_helper::ErrorThrower thrower,
                                           gin::Arguments* args) {
  if (!Browser::Get()->is_ready()) {
    thrower.ThrowError("Cannot create ScrollView before app is ready");
    return nullptr;
  }

  gin::Dictionary options = gin::Dictionary::CreateEmpty(args->isolate());
  args->GetNext(&options);

  return new ScrollView(args, options);
}

// static
void ScrollView::BuildPrototype(v8::Isolate* isolate,
                                v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "ScrollView"));
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("setContentView", &ScrollView::SetContentView)
      .SetMethod("getContentView", &ScrollView::GetContentView)
      .SetMethod("setContentSize", &ScrollView::SetContentSize)
      .SetMethod("getContentSize", &ScrollView::GetContentSize)
      .SetMethod("setHorizontalScrollBarMode",
                 &ScrollView::SetHorizontalScrollBarMode)
      .SetMethod("getHorizontalScrollBarMode",
                 &ScrollView::GetHorizontalScrollBarMode)
      .SetMethod("setVerticalScrollBarMode",
                 &ScrollView::SetVerticalScrollBarMode)
      .SetMethod("getVerticalScrollBarMode",
                 &ScrollView::GetVerticalScrollBarMode)
      .SetMethod("setScrollerInsets", &ScrollView::SetScrollerInsets)
      .SetMethod("setScrollWheelSwapped", &ScrollView::SetScrollWheelSwapped)
      .SetMethod("isScrollWheelSwapped", &ScrollView::IsScrollWheelSwapped)
      .SetMethod("setScrollEventsEnabled", &ScrollView::SetScrollEventsEnabled)
      .SetMethod("isScrollEventsEnabled", &ScrollView::IsScrollEventsEnabled)
      .SetMethod("setHorizontalScrollElasticity",
                 &ScrollView::SetHorizontalScrollElasticity)
      .SetMethod("getHorizontalScrollElasticity",
                 &ScrollView::GetHorizontalScrollElasticity)
      .SetMethod("setVerticalScrollElasticity",
                 &ScrollView::SetVerticalScrollElasticity)
      .SetMethod("getVerticalScrollElasticity",
                 &ScrollView::GetVerticalScrollElasticity)
      .SetMethod("setScrollPosition", &ScrollView::SetScrollPosition)
      .SetMethod("getScrollPosition", &ScrollView::GetScrollPosition)
      .SetMethod("getMaximumScrollPosition",
                 &ScrollView::GetMaximumScrollPosition)
#if BUILDFLAG(IS_MAC)
      .SetMethod("scrollToPoint", &ScrollView::ScrollToPoint)
      .SetMethod("scrollPointToCenter", &ScrollView::ScrollPointToCenter)
      .SetMethod("setOverlayScrollbar", &ScrollView::SetOverlayScrollbar)
      .SetMethod("isOverlayScrollbar", &ScrollView::IsOverlayScrollbar)
      .SetMethod("setScrollWheelFactor", &ScrollView::SetScrollWheelFactor)
      .SetMethod("getScrollWheelFactor", &ScrollView::GetScrollWheelFactor)
#endif
#if !BUILDFLAG(IS_MAC)
      .SetMethod("clipHeightTo", &ScrollView::ClipHeightTo)
      .SetMethod("getMinHeight", &ScrollView::GetMinHeight)
      .SetMethod("getMaxHeight", &ScrollView::GetMaxHeight)
      .SetMethod("scrollRectToVisible", &ScrollView::ScrollRectToVisible)
      .SetMethod("getVisibleRect", &ScrollView::GetVisibleRect)
      .SetMethod("setAllowKeyboardScrolling",
                 &ScrollView::SetAllowKeyboardScrolling)
      .SetMethod("getAllowKeyboardScrolling",
                 &ScrollView::GetAllowKeyboardScrolling)
      .SetMethod("setDrawOverflowIndicator",
                 &ScrollView::SetDrawOverflowIndicator)
      .SetMethod("getDrawOverflowIndicator",
                 &ScrollView::GetDrawOverflowIndicator)
#endif
      .Build();
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::ScrollView;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("ScrollView", gin_helper::CreateConstructor<ScrollView>(
                             isolate, base::BindRepeating(&ScrollView::New)));
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_scroll_view, Initialize)
