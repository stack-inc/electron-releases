// Copyright (c) 2017 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/native_browser_view_mac.h"

#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "shell/browser/ui/view_utils.h"
#include "skia/ext/skia_utils_mac.h"
#include "ui/gfx/geometry/rect.h"

// Match view::Views behavior where the view sticks to the top-left origin.
const NSAutoresizingMaskOptions kDefaultAutoResizingMask =
    NSViewMaxXMargin | NSViewMinYMargin;

namespace electron {

NativeBrowserViewMac::NativeBrowserViewMac(
    InspectableWebContents* inspectable_web_contents)
    : NativeBrowserView(inspectable_web_contents) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = kDefaultAutoResizingMask;
}

NativeBrowserViewMac::~NativeBrowserViewMac() = default;

void NativeBrowserViewMac::SetAutoResizeFlags(uint8_t flags) {
  NSAutoresizingMaskOptions autoresizing_mask = kDefaultAutoResizingMask;
  if (flags & kAutoResizeWidth) {
    autoresizing_mask |= NSViewWidthSizable;
  }
  if (flags & kAutoResizeHeight) {
    autoresizing_mask |= NSViewHeightSizable;
  }
  if (flags & kAutoResizeHorizontal) {
    autoresizing_mask |=
        NSViewMaxXMargin | NSViewMinXMargin | NSViewWidthSizable;
  }
  if (flags & kAutoResizeVertical) {
    autoresizing_mask |=
        NSViewMaxYMargin | NSViewMinYMargin | NSViewHeightSizable;
  }

  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = autoresizing_mask;
}

void NativeBrowserViewMac::SetBounds(const gfx::Rect& bounds) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  auto* superview = view.superview;
  const auto superview_height = superview ? superview.frame.size.height : 0;

  // We need to use the content rect to calculate the titlebar height if the
  // superview is an framed NSWindow, otherwise it will be offset incorrectly by
  // the height of the titlebar.
  auto titlebar_height = 0;
  if (auto* win = [superview window]) {
    const auto content_rect_height =
        [win contentRectForFrameRect:superview.frame].size.height;
    titlebar_height = superview_height - content_rect_height;
  }

  auto new_height =
      superview_height - bounds.y() - bounds.height() + titlebar_height;
  view.frame =
      NSMakeRect(bounds.x(), new_height, bounds.width(), bounds.height());
}

void NativeBrowserViewMac::SetBounds(const gfx::Rect& bounds,
                                     const BoundsAnimationOptions& options) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  SetBoundsForView(view, bounds, options);
}

gfx::Rect NativeBrowserViewMac::GetBounds() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return gfx::Rect();
  NSView* view = iwc_view->GetNativeView().GetNativeNSView();
  auto* superview = view.superview;
  const int superview_height = superview ? superview.frame.size.height : 0;

  // We need to use the content rect to calculate the titlebar height if the
  // superview is an framed NSWindow, otherwise it will be offset incorrectly by
  // the height of the titlebar.
  auto titlebar_height = 0;
  if (auto* win = [superview window]) {
    const auto content_rect_height =
        [win contentRectForFrameRect:superview.frame].size.height;
    titlebar_height = superview_height - content_rect_height;
  }

  auto new_height = superview_height - view.frame.origin.y -
                    view.frame.size.height + titlebar_height;
  return gfx::Rect(view.frame.origin.x, new_height, view.frame.size.width,
                   view.frame.size.height);
}

void NativeBrowserViewMac::SetBackgroundColor(SkColor color) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.wantsLayer = YES;
  view.layer.backgroundColor = skia::CGColorCreateFromSkColor(color);
}

void NativeBrowserViewMac::SetViewBounds(const gfx::Rect& bounds) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.bounds =
      NSMakeRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
  [view setNeedsDisplay:YES];
}

gfx::Rect NativeBrowserViewMac::GetViewBounds() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return gfx::Rect();
  NSView* view = iwc_view->GetNativeView().GetNativeNSView();
  return gfx::Rect(view.bounds.origin.x, view.bounds.origin.y,
                   view.bounds.size.width, view.bounds.size.height);
}

void NativeBrowserViewMac::ResetScaling() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  ResetScalingForView(view);
}

void NativeBrowserViewMac::SetScale(const ScaleAnimationOptions& options) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  SetScaleForView(view, options);
}

float NativeBrowserViewMac::GetScaleX() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return 1.0;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  return GetScaleXForView(view);
}

float NativeBrowserViewMac::GetScaleY() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return 1.0;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  return GetScaleYForView(view);
}

void NativeBrowserViewMac::SetOpacity(const double opacity,
                                      const AnimationOptions& options) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  SetOpacityForView(view, opacity, options);
}

double NativeBrowserViewMac::GetOpacity() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return 1.0;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  return GetOpacityForView(view);
}

void NativeBrowserViewMac::SetVisible(bool visible) {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.hidden = !visible;
}

bool NativeBrowserViewMac::IsVisible() {
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return false;
  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  return !view.hidden;
}

// static
NativeBrowserView* NativeBrowserView::Create(
    InspectableWebContents* inspectable_web_contents) {
  return new NativeBrowserViewMac(inspectable_web_contents);
}

}  // namespace electron
