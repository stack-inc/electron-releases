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
#include "shell/browser/draggable_region_provider.h"
#include "shell/browser/extended_web_contents_observer.h"
#include "shell/browser/native_window.h"
#include "shell/common/api/api.mojom.h"
#include "shell/common/gin_helper/pinnable.h"

#if !BUILDFLAG(IS_MAC)
#include "base/callback.h"
#include "content/public/browser/render_widget_host.h"

namespace blink {
class WebMouseEvent;
}
#endif

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
                       public DraggableRegionProvider,
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

  // DraggableRegionProvider:
  int NonClientHitTest(const gfx::Point& point) override;

  // content::WebContentsObserver:
  void WebContentsDestroyed() override;
#if !BUILDFLAG(IS_MAC)
  void RenderViewReady() override;
  void RenderFrameCreated(content::RenderFrameHost* host) override;
  void RenderFrameDeleted(content::RenderFrameHost* host) override;
  void RenderFrameHostChanged(content::RenderFrameHost* old_host,
                              content::RenderFrameHost* new_host) override;
#endif

  // ExtendedWebContentsObserver:
  void OnCloseContents() override;

  // BaseView:
#if BUILDFLAG(IS_MAC)
  void SetBounds(const gfx::Rect& bounds, gin::Arguments* args) override;
#endif
  void SetBackgroundColorImpl(const SkColor& color) override;
  void SetWindow(BaseWindow* window) override;
#if !BUILDFLAG(IS_MAC)
  void SetRoundedCorners(const RoundedCornersOptions& options) override;
  void UpdateClickThrough() override;
#endif

  // WebBrowserView APIs.
  void Hide(bool freeze, gfx::Image thumbnail);
  void Show();
  v8::Local<v8::Value> GetWebContents(v8::Isolate*);

  // Helpers.

  void CreateWebBrowserView(InspectableWebContents* inspectable_web_contents);

  InspectableWebContentsView* GetInspectableWebContentsView();

#if !BUILDFLAG(IS_MAC)
  void AttachToHost(content::RenderFrameHost* host);
  void DetachFromHost();
  bool HandleMouseEvent(const blink::WebMouseEvent& event);
#endif

 private:
  v8::Global<v8::Value> web_contents_;
  class WebContents* api_web_contents_ = nullptr;

  base::WeakPtr<BaseWindow> owner_window_;

  bool page_frozen_ = false;

#if !BUILDFLAG(IS_MAC)
  content::RenderWidgetHost::MouseEventCallback mouse_event_callback_;
  content::RenderWidgetHost* host_ = nullptr;
#endif
};

}  // namespace api

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_
