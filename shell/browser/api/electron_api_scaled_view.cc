// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_scaled_view.h"

#include "base/no_destructor.h"
#include "gin/handle.h"
#include "shell/browser/browser.h"
#include "shell/common/gin_converters/gfx_converter.h"
#include "shell/common/gin_converters/value_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"

namespace electron {

namespace api {

ScaledView::ScaledView(gin::Arguments* args, bool vibrant, bool blurred)
    : BaseView(vibrant, blurred) {
  CreateScaledView();
  InitWithArgs(args);
}

ScaledView::~ScaledView() = default;

// static
gin_helper::WrappableBase* ScaledView::New(gin_helper::ErrorThrower thrower,
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

  return new ScaledView(args, vibrant, blurred);
}

// static
v8::Local<v8::Function> ScaledView::GetConstructor(v8::Isolate* isolate) {
  static base::NoDestructor<v8::Global<v8::Function>> constructor;
  if (constructor.get()->IsEmpty()) {
    constructor->Reset(isolate, gin_helper::CreateConstructor<ScaledView>(
                                    isolate, base::BindRepeating(&ScaledView::New)));
  }
  return v8::Local<v8::Function>::New(isolate, *constructor.get());
}

// static
gin::Handle<ScaledView> ScaledView::Create(v8::Isolate* isolate) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> obj;
  if (GetConstructor(isolate)->NewInstance(context, 0, nullptr).ToLocal(&obj)) {
    gin::Handle<ScaledView> view;
    if (gin::ConvertFromV8(isolate, obj, &view))
      return view;
  }
  return gin::Handle<ScaledView>();
}

// static
void ScaledView::BuildPrototype(v8::Isolate* isolate,
                              v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "ScaledView"));
  gin_helper::Destroyable::MakeDestroyable(isolate, prototype);
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("setZoomFactor", &ScaledView::SetZoomFactor)
      .SetMethod("getZoomFactor", &ScaledView::GetZoomFactor)
      .Build();
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::ScaledView;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("ScaledView", ScaledView::GetConstructor(isolate));
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_scaled_view, Initialize)
