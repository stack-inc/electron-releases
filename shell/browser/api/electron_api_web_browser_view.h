// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_
#define ELECTRON_SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_

#include <memory>
#include <string>
#include <vector>

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "shell/browser/api/electron_api_base_view.h"
#include "shell/browser/extended_web_contents_observer.h"
#include "shell/browser/native_window.h"
#include "shell/common/api/api.mojom.h"
#include "shell/common/gin_helper/pinnable.h"

namespace gfx {
class Image;
}

namespace gin_helper {
class Dictionary;
}

namespace electron {

class InspectableWebContents;
class InspectableWebContentsView;

namespace api {

class BaseWindow;
class WebContents;

class WebBrowserView : public BaseView,
                       public gin_helper::Pinnable<WebBrowserView>,
                       public content::WebContentsObserver,
                       public ExtendedWebContentsObserver {
 public:
  static gin_helper::WrappableBase* New(gin_helper::ErrorThrower thrower,
                                        gin::Arguments* args);

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

  // disable copy
  WebBrowserView(const WebBrowserView&) = delete;
  WebBrowserView& operator=(const WebBrowserView&) = delete;

  WebContents* web_contents() const { return api_web_contents_; }

 protected:
  WebBrowserView(gin::Arguments* args, gin::Handle<WebContents> web_contents);
  ~WebBrowserView() override;

  void CreateWebBrowserView(InspectableWebContents* inspectable_web_contents);

  // content::WebContentsObserver:
  void WebContentsDestroyed() override;
#if !BUILDFLAG(IS_MAC)
  void RenderViewReady() override;
#endif

  // ExtendedWebContentsObserver:
  void OnDraggableRegionsUpdated(
      const std::vector<mojom::DraggableRegionPtr>& regions) override;

  // BaseView:
#if BUILDFLAG(IS_MAC)
  void SetBounds(const gfx::Rect& bounds, gin::Arguments* args) override;
#endif
  void SetBackgroundColorImpl(const SkColor& color) override;
  void SetWindowForChildren(BaseWindow* window) override;
#if !BUILDFLAG(IS_MAC)
  void SetRoundedCorners(const RoundedCornersOptions& options) override;
  void UpdateClickThrough() override;
#endif

  // WebBrowserView APIs.
  void Hide(bool freeze, gfx::Image thumbnail);
  void Show();
  v8::Local<v8::Value> GetWebContents(v8::Isolate*);

  // Helpers.

  InspectableWebContentsView* GetInspectableWebContentsView();

 private:
  v8::Global<v8::Value> web_contents_;
  class WebContents* api_web_contents_ = nullptr;

  base::WeakPtr<BaseWindow> owner_window_;

  bool page_frozen_ = false;
};

}  // namespace api

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_
