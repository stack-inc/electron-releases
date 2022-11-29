// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCALED_VIEW_H_
#define ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCALED_VIEW_H_

#include <memory>
#include <string>
#include <vector>

#include "shell/browser/api/electron_api_base_view.h"

namespace electron {

namespace api {

class ScaledView : public BaseView {
 public:
  static gin_helper::WrappableBase* New(gin_helper::ErrorThrower thrower,
                                        gin::Arguments* args);
  static gin::Handle<ScaledView> Create(v8::Isolate* isolate);

  // Return the cached constructor function.
  static v8::Local<v8::Function> GetConstructor(v8::Isolate* isolate);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  // disable copy
  ScaledView(const ScaledView&) = delete;
  ScaledView& operator=(const ScaledView&) = delete;

 protected:
  ScaledView(gin::Arguments* args, bool vibrant, bool blurred);
  ~ScaledView() override;

  void CreateScaledView();

  // ScaledView APIs.
  void SetZoomFactor(float factor);
  float GetZoomFactor() const;

  // Helpers.

 private:
};

}  // namespace api

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_API_ELECTRON_API_SCALED_VIEW_H_
