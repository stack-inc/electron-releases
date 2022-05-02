
#include "electron/shell/browser/ui/views/scroll_with_layers/web_view_scroll_with_layers.h"

#include "electron/shell/browser/ui/views/inspectable_web_contents_view_views.h"

namespace electron {

void WebViewScrollWithLayers::DidFirstVisuallyNonEmptyPaint() {
  if (!iwcvv_)
      return;

  iwcvv_->SetStopPaintBackground(true);
}

}  // namespace electron
