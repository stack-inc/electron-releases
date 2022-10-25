// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_web_browser_view.h"

#include "base/bind.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "shell/browser/api/electron_api_base_window.h"
#include "shell/browser/api/electron_api_web_contents.h"
#include "shell/browser/browser.h"
#include "shell/browser/native_window.h"
#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "third_party/blink/public/common/input/web_mouse_event.h"
#include "ui/compositor/layer.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/rounded_corners_f.h"
#include "ui/views/view.h"

#if BUILDFLAG(IS_MAC)
#include "shell/browser/ui/cocoa/delayed_native_view_host.h"
#endif

namespace electron::api {

void WebBrowserView::RenderViewReady() {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
  GetView()->Layout();
#else
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->GetView()->Layout();
#endif
}

void WebBrowserView::RenderFrameCreated(content::RenderFrameHost* frame_host) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  // Only handle the initial main frame, not speculative ones.
  if (frame_host != web_contents()->GetWebContents()->GetPrimaryMainFrame())
    return;
  DCHECK(!host_);

  AttachToHost(frame_host);
}

void WebBrowserView::RenderFrameDeleted(content::RenderFrameHost* frame_host) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  // Only handle the active main frame, not speculative ones.
  if (frame_host != web_contents()->GetWebContents()->GetPrimaryMainFrame())
    return;
  DCHECK(host_);
  DCHECK_EQ(host_, frame_host->GetRenderWidgetHost());

  DetachFromHost();
}

void WebBrowserView::RenderFrameHostChanged(
    content::RenderFrameHost* old_host,
    content::RenderFrameHost* new_host) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage())
    return;
#endif
  // Since we skipped speculative main frames in RenderFrameCreated, we must
  // watch for them being swapped in by watching for RenderFrameHostChanged().
  if (new_host != web_contents()->GetWebContents()->GetPrimaryMainFrame())
    return;
  // Don't watch for the initial main frame RenderFrameHost, which does not come
  // with a renderer frame. We'll hear about that from RenderFrameCreated.
  if (!old_host) {
    // If this fails, then we need to AttachToHost() here when the `new_host`
    // has its renderer frame. Since `old_host` is null only when this observer
    // method is called at startup, it should be before the renderer frame is
    // created.
    DCHECK(!new_host->IsRenderFrameLive());
    return;
  }
  DCHECK(host_);
  DCHECK_EQ(host_, old_host->GetRenderWidgetHost());

  DetachFromHost();
  AttachToHost(new_host);
}

void WebBrowserView::SetRoundedCorners(const RoundedCornersOptions& options) {
  BaseView::SetRoundedCorners(options);
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    WebBrowserView::SetRoundedCornersMac(options);
    return;
  }
#else
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  auto* view = iwc_view->GetView();
  view->SetPaintToLayer();
  view->layer()->SetFillsBoundsOpaquely(false);
  float radius = options.radius;
  auto corner_radii = gfx::RoundedCornersF(options.top_left ? radius : 0.0f,
                                           options.top_right ? radius : 0.0f,
                                           options.bottom_right ? radius : 0.0f,
                                           options.bottom_left ? radius : 0.0f);
  view->layer()->SetRoundedCornerRadius(corner_radii);
  view->layer()->SetIsFastRoundedCorner(true);
  iwc_view->SetCornerRadii(corner_radii);
#endif  // else BUILDFLAG(IS_MAC)
}

void WebBrowserView::UpdateClickThrough() {
  bool click_through = IsClickThrough();
  if (GetView())
    GetView()->SetCanProcessEventsWithinSubtree(!click_through);

#if BUILDFLAG(IS_MAC)
  WebBrowserView::SetClickThroughMac(click_through);
#else
  InspectableWebContentsView* iwc_view = GetInspectableWebContentsView();
  if (iwc_view)
    iwc_view->SetClickThrough(click_through);
#endif
}

void WebBrowserView::CreateWebBrowserView(
    InspectableWebContents* inspectable_web_contents) {
#if BUILDFLAG(IS_MAC)
  if (!Browser::Get()->IsViewsUsage()) {
    CreateWebBrowserViewMac(inspectable_web_contents);
    return;
  }
#endif
  InspectableWebContentsView* iwc_view =
      inspectable_web_contents ? inspectable_web_contents->GetView() : nullptr;
  views::View* view = nullptr;
  if (iwc_view) {
#if BUILDFLAG(IS_MAC)
    view = new DelayedNativeViewHost(iwc_view->GetNativeView());
#else
    view = iwc_view->GetView();
    // On macOS the View is a newly-created |DelayedNativeViewHost| and it is
    // our responsibility to delete it. On other platforms the View is created
    // and managed by InspectableWebContents.
    set_delete_view(false);
#endif
  } else {
    view = new views::View();
  }

  SetView(view);
}

void WebBrowserView::AttachToHost(content::RenderFrameHost* frame_host) {
  DCHECK(frame_host->IsRenderFrameLive());
  // Historically, (see https://crbug.com/847363) this code handled the
  // RenderWidgetHostView being null, but now it is listening to creation of the
  // frame which includes creation of the widget so it is implied that
  // RenderWidgetHostView exists.
  DCHECK(frame_host->GetView());

  host_ = frame_host->GetView()->GetRenderWidgetHost();
  host_->AddMouseEventCallback(mouse_event_callback_);
}

void WebBrowserView::DetachFromHost() {
  host_->RemoveMouseEventCallback(mouse_event_callback_);
  host_ = nullptr;
}

bool WebBrowserView::HandleMouseEvent(const blink::WebMouseEvent& event) {
  if (!AreMouseEventsEnabled())
    return false;

  MouseEventType type = MouseEventType::kUnknown;
  switch (event.GetType()) {
    case blink::WebInputEvent::Type::kMouseDown:
      type = MouseEventType::kDown;
      break;
    case blink::WebInputEvent::Type::kMouseUp:
      type = MouseEventType::kUp;
      break;
    case blink::WebInputEvent::Type::kMouseMove:
      if (!IsMouseTrackingEnabled())
        return false;
      type = MouseEventType::kMove;
      break;
    case blink::WebInputEvent::Type::kMouseEnter:
      if (!IsMouseTrackingEnabled())
        return false;
      type = MouseEventType::kEnter;
      break;
    case blink::WebInputEvent::Type::kMouseLeave:
      if (!IsMouseTrackingEnabled())
        return false;
      type = MouseEventType::kLeave;
      break;
    default:
      break;
  }
  if (type == MouseEventType::kUnknown)
    return false;

  uint32_t time_stamp = static_cast<uint32_t>(
      (event.TimeStamp() - base::TimeTicks()).InMillisecondsF());

  int button = 0;
  if (event.button == blink::WebMouseEvent::Button::kLeft)
    button = 1;
  else if (event.button == blink::WebMouseEvent::Button::kRight)
    button = 2;
  else if (event.button == blink::WebMouseEvent::Button::kMiddle)
    button = 3;

  gfx::Point position_in_view(event.PositionInWidget().x(),
                              event.PositionInWidget().y());
  gfx::Point position_in_window =
      display::Screen::GetScreen()->GetCursorScreenPoint();
  views::View* root_view = GetWindow()->window()->GetRootView();
  views::View::ConvertPointFromScreen(root_view, &position_in_window);

  return NotifyMouseEvent(type, time_stamp, button, position_in_view,
                          position_in_window);
}

}  // namespace electron::api
