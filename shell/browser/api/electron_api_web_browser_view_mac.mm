// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_web_browser_view.h"

#include "shell/browser/browser.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/gfx/geometry/rect.h"

// Match view::Views behavior where the view sticks to the top-left origin.
const NSAutoresizingMaskOptions kDefaultAutoResizingMask =
    NSViewMaxXMargin | NSViewMinYMargin;

@interface NSView (ElectronCustomMethods)
- (void)setClickThrough:(BOOL)click_through;
@end

namespace electron::api {

void WebBrowserView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  BaseView::SetBounds(bounds, args);
  if (Browser::Get()->IsViewsUsage())
    return;

  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.frame = NSMakeRect(0, 0, bounds.width(), bounds.height());

  // Ensure draggable regions are properly updated to reflect new bounds.
  iwc_view->UpdateDraggableRegions(iwc_view->GetDraggableRegions());
}

void WebBrowserView::SetClickThroughMac(bool click_through) {
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* view = iwc_view->GetNativeView().GetNativeNSView();
  DCHECK([view respondsToSelector:@selector(setClickThrough:)])
      << "inspectable_view is not an instance of "
         " the ElectronInspectableWebContentsView"
         " which is the only NSView which responds to the "
         "setClickThrough.";
  [view setClickThrough:(click_through ? YES : NO)];
}

void WebBrowserView::SetRoundedCornersMac(
    const RoundedCornersOptions& options) {
  if (@available(macOS 10.13, *)) {
    auto* iwc_view = GetInspectableWebContentsView();
    if (!iwc_view)
      return;

    auto* view = iwc_view->GetNativeView().GetNativeNSView();

    SetWantsLayer(true);
    view.layer.masksToBounds = YES;
    view.layer.cornerRadius = options.radius;

    CACornerMask mask = 0;
    if (options.top_left)
      mask |= kCALayerMinXMinYCorner;
    if (options.top_right)
      mask |= kCALayerMaxXMinYCorner;
    if (options.bottom_left)
      mask |= kCALayerMinXMaxYCorner;
    if (options.bottom_right)
      mask |= kCALayerMaxXMaxYCorner;
    view.layer.maskedCorners = mask;
  }
}

void WebBrowserView::CreateWebBrowserViewMac(
    InspectableWebContents* inspectable_web_contents) {
  SetView([[ElectronNativeView alloc] init]);

  InspectableWebContentsView* iwc_view =
      inspectable_web_contents ? inspectable_web_contents->GetView() : nullptr;
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = kDefaultAutoResizingMask;
  [GetNSView() addSubview:view];
}

}  // namespace electron::api
