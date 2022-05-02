// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_WITH_LAYERS_WEB_VIEW_SCROLL_WITH_LAYERS_H_
#define ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_WITH_LAYERS_WEB_VIEW_SCROLL_WITH_LAYERS_H_

#include "ui/views/controls/webview/webview.h"

#include <memory>

namespace electron {

class InspectableWebContentsViewViews;

class WebViewScrollWithLayers : public views::WebView {
 public:
  WebViewScrollWithLayers(InspectableWebContentsViewViews* iwcvv,
      std::unique_ptr<views::NativeViewHost> holder,
      content::BrowserContext* browser_context)
          : views::WebView(std::move(holder), browser_context),
            iwcvv_(iwcvv) {}

  WebViewScrollWithLayers(const WebViewScrollWithLayers&) = delete;
  WebViewScrollWithLayers(WebViewScrollWithLayers&&) = delete;

  WebViewScrollWithLayers& operator=(const WebViewScrollWithLayers&) = delete;
  WebViewScrollWithLayers& operator=(WebViewScrollWithLayers&&) = delete;

 protected:
  void DidFirstVisuallyNonEmptyPaint() override;

 private:
  InspectableWebContentsViewViews* iwcvv_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_VIEWS_SCROLL_WITH_LAYERS_WEB_VIEW_SCROLL_WITH_LAYERS_H_
