#include "shell/browser/api/electron_api_web_browser_view.h"

#include <vector>

#include "content/browser/renderer_host/render_widget_host_view_base.h"  // nogncheck
#include "content/browser/web_contents/web_contents_impl.h"  // nogncheck
#include "content/public/browser/render_widget_host_view.h"
#include "gin/handle.h"
#include "shell/browser/api/electron_api_web_contents.h"
#include "shell/browser/browser.h"
#include "shell/browser/web_contents_preferences.h"
#include "shell/common/gin_converters/image_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"
#include "shell/common/options_switches.h"

namespace electron {

namespace api {

WebBrowserView::WebBrowserView(gin::Arguments* args,
                               gin::Handle<WebContents> web_contents,
                               NativeWebBrowserView* view)
    : BaseView(args->isolate(), view), view_(view) {
  InitWithArgs(args);

  web_contents_.Reset(args->isolate(), web_contents.ToV8());
  api_web_contents_ = web_contents.get();
  api_web_contents_->AddObserver(this);
  Observe(api_web_contents_->web_contents());
  view_->AddObserver(this);
}

WebBrowserView::~WebBrowserView() {
  if (web_contents()) {  // destroy() called without closing WebContents
    web_contents()->RemoveObserver(this);
    web_contents()->Destroy();
  }
}

void WebBrowserView::OnSettingOwnerWindow(NativeWindow* window) {
  // Ensure WebContents and BrowserView owner windows are in sync.
  if (web_contents())
    web_contents()->SetOwnerWindow(window);

  if (owner_window_.get()) {
    owner_window_->remove_inspectable_view(
        view_->GetInspectableWebContentsView());
  }

  owner_window_ = window ? window->GetWeakPtr() : nullptr;

  if (owner_window_.get() && view_->GetInspectableWebContentsView())
    owner_window_->add_inspectable_view(view_->GetInspectableWebContentsView());
}

void WebBrowserView::WebContentsDestroyed() {
  api_web_contents_ = nullptr;
  web_contents_.Reset();
  Unpin();
}

void WebBrowserView::OnDraggableRegionsUpdated(
    const std::vector<mojom::DraggableRegionPtr>& regions) {
  InspectableWebContentsView* iwc_view = view_->GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  iwc_view->UpdateDraggableRegions(regions);
}

void WebBrowserView::SetBackgroundColorImpl(const SkColor& color) {
  if (web_contents()) {
    auto* wc = web_contents()->web_contents();
    wc->SetPageBaseBackgroundColor(color);

    auto* const rwhv = wc->GetRenderWidgetHostView();
    if (rwhv) {
      rwhv->SetBackgroundColor(color);
      static_cast<content::RenderWidgetHostViewBase*>(rwhv)
          ->SetContentBackgroundColor(color);
    }

    // Ensure new color is stored in webPreferences, otherwise
    // the color will be reset on the next load via HandleNewRenderFrame.
    auto* web_preferences = WebContentsPreferences::From(wc);
    if (web_preferences)
      web_preferences->SetBackgroundColor(color);
  }
}

void WebBrowserView::Hide(bool freeze, gfx::Image thumbnail) {
  if (freeze && !page_frozen_) {
    auto* wc =
        static_cast<content::WebContentsImpl*>(web_contents()->web_contents());
    wc->WasHidden();
    wc->SetPageFrozen(true);
    page_frozen_ = true;
  }
  view_->ShowThumbnail(thumbnail);
}

void WebBrowserView::Show() {
  if (page_frozen_) {
    auto* wc =
        static_cast<content::WebContentsImpl*>(web_contents()->web_contents());
    wc->SetPageFrozen(false);
    wc->WasShown();
    page_frozen_ = false;
  }
  view_->HideThumbnail();
}

v8::Local<v8::Value> WebBrowserView::GetWebContents(v8::Isolate* isolate) {
  if (web_contents_.IsEmpty()) {
    return v8::Null(isolate);
  }

  return v8::Local<v8::Value>::New(isolate, web_contents_);
}

// static
gin_helper::WrappableBase* WebBrowserView::New(gin_helper::ErrorThrower thrower,
                                               gin::Arguments* args) {
  if (!Browser::Get()->is_ready()) {
    thrower.ThrowError("Cannot create WebBrowserView before app is ready");
    return nullptr;
  }

  v8::Isolate* isolate = args->isolate();
  gin::Dictionary options = gin::Dictionary::CreateEmpty(isolate);
  args->GetNext(&options);

  gin_helper::Dictionary web_preferences =
      gin::Dictionary::CreateEmpty(isolate);
  options.Get(options::kWebPreferences, &web_preferences);
  web_preferences.Set("type", "browserView");

  v8::Local<v8::Value> value;

  // Copy the webContents option to webPreferences.
  if (options.Get("webContents", &value)) {
    web_preferences.SetHidden("webContents", value);
  }

  auto web_contents =
      WebContents::CreateFromWebPreferences(isolate, web_preferences);

  NativeWebBrowserView* view =
      new NativeWebBrowserView(web_contents->inspectable_web_contents());

  auto* web_browser_view = new WebBrowserView(args, web_contents, view);
  web_browser_view->Pin(isolate);
  return web_browser_view;
}

// static
void WebBrowserView::BuildPrototype(v8::Isolate* isolate,
                                    v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "WebBrowserView"));
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("hide", &WebBrowserView::Hide)
      .SetMethod("show", &WebBrowserView::Show)
      .SetProperty("webContents", &WebBrowserView::GetWebContents)
      .Build();
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::WebBrowserView;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("WebBrowserView",
           gin_helper::CreateConstructor<WebBrowserView>(
               isolate, base::BindRepeating(&WebBrowserView::New)));
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_web_browser_view, Initialize)
