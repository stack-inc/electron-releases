#ifndef SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_
#define SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_

#include <memory>
#include <string>
#include <vector>

#include "content/public/browser/web_contents_observer.h"
#include "shell/browser/api/electron_api_base_view.h"
#include "shell/browser/extended_web_contents_observer.h"
#include "shell/browser/native_window.h"
#include "shell/browser/ui/native_web_browser_view.h"
#include "shell/common/api/api.mojom.h"
#include "shell/common/gin_helper/pinnable.h"
#include "ui/gfx/image/image.h"

namespace gin_helper {
class Dictionary;
}

namespace electron {

namespace api {

class WebContents;

class WebBrowserView : public BaseView,
                       public gin_helper::Pinnable<WebBrowserView>,
                       public NativeWebBrowserView::Observer,
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
  WebBrowserView(gin::Arguments* args,
                 gin::Handle<WebContents> web_contents,
                 NativeWebBrowserView* view);
  ~WebBrowserView() override;

  // NativeWebBrowserView:
  void OnSettingOwnerWindow(NativeWindow* window) override;

  // content::WebContentsObserver:
  void WebContentsDestroyed() override;

  // ExtendedWebContentsObserver:
  void OnDraggableRegionsUpdated(
      const std::vector<mojom::DraggableRegionPtr>& regions) override;

  // BaseView:
  void SetBackgroundColorImpl(const SkColor& color) override;

 private:
  void Hide(bool freeze, gfx::Image thumbnail);
  void Show();
  v8::Local<v8::Value> GetWebContents(v8::Isolate*);

  v8::Global<v8::Value> web_contents_;
  class WebContents* api_web_contents_ = nullptr;

  NativeWebBrowserView* view_;
  base::WeakPtr<NativeWindow> owner_window_;

  bool page_frozen_ = false;
};

}  // namespace api

}  // namespace electron

#endif  // SHELL_BROWSER_API_ELECTRON_API_WEB_BROWSER_VIEW_H_
