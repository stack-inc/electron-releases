#include "shell/browser/ui/native_web_browser_view.h"

#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/gfx/geometry/rect.h"

// Match view::Views behavior where the view sticks to the top-left origin.
const NSAutoresizingMaskOptions kDefaultAutoResizingMask =
    NSViewMaxXMargin | NSViewMinYMargin;

namespace electron {

void NativeWebBrowserView::InitWebBrowserView() {
  SetNativeView([[ElectronNativeView alloc] init]);

  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = kDefaultAutoResizingMask;
  [GetNative() addSubview:view];
}

void NativeWebBrowserView::SetBounds(const gfx::Rect& bounds,
                                     const BoundsAnimationOptions& options) {
  NativeView::SetBounds(bounds, options);

  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.frame = NSMakeRect(0, 0, bounds.width(), bounds.height());

  // Ensure draggable regions are properly updated to reflect new bounds.
  iwc_view->UpdateDraggableRegions(iwc_view->GetDraggableRegions());
}

}  // namespace electron
