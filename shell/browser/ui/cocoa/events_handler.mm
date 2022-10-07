#include "shell/browser/ui/cocoa/events_handler.h"

#include <objc/objc-runtime.h>

#include "base/notreached.h"
#include "shell/browser/api/electron_api_base_view.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"

namespace electron {

namespace {

api::BaseView* CheckAndGetView(NSView* self) {
  CHECK([self respondsToSelector:@selector(shell)])
      << "Handler called for view other than ElectronNativeView";
  api::BaseView* view = [self shell];
  DCHECK(view) << "Handler called after view is destroyed";
  return view;
}

bool NativeDummy(NSView* self, SEL _cmd) {
  return true;
}

void OnMouseEvent(NSView* self, SEL _cmd, NSEvent* event) {
  api::BaseView* view = CheckAndGetView(self);
  if (!view)
    return;

  // Emit the event to View.
  if (!DispatchMouseEvent(view, event)) {
    // Transfer the event to super class.
    auto super_impl = reinterpret_cast<void (*)(NSView*, SEL, NSEvent*)>(
        [[self superclass] instanceMethodForSelector:_cmd]);
    super_impl(self, _cmd, event);
  }
}

api::BaseView::MouseEventType MouseEventTypeFromNS(NSEvent* event) {
  switch ([event type]) {
    case NSEventTypeLeftMouseDown:
    case NSEventTypeRightMouseDown:
    case NSEventTypeOtherMouseDown:
      return api::BaseView::MouseEventType::kDown;
    case NSEventTypeLeftMouseUp:
    case NSEventTypeRightMouseUp:
    case NSEventTypeOtherMouseUp:
      return api::BaseView::MouseEventType::kUp;
    case NSEventTypeLeftMouseDragged:
    case NSEventTypeRightMouseDragged:
    case NSEventTypeOtherMouseDragged:
    case NSEventTypeMouseMoved:
      return api::BaseView::MouseEventType::kMove;
    case NSEventTypeMouseEntered:
      return api::BaseView::MouseEventType::kEnter;
    case NSEventTypeMouseExited:
      return api::BaseView::MouseEventType::kLeave;
    default:
      return api::BaseView::MouseEventType::kUnknown;
  }
}

gfx::Point GetPosInView(NSEvent* event, NSView* view) {
  NSPoint point = [view convertPoint:[event locationInWindow] fromView:nil];
  if ([view isFlipped])
    return gfx::Point(point.x, point.y);
  NSRect frame = [view frame];
  return gfx::Point(point.x, NSHeight(frame) - point.y);
}

gfx::Point GetPosInWindow(NSEvent* event, NSView* view) {
  NSPoint point = [event locationInWindow];
  if ([view isFlipped])
    return gfx::Point(point.x, point.y);
  NSWindow* window = [event window];
  NSRect frame = [window contentRectForFrameRect:[window frame]];
  return gfx::Point(point.x, NSHeight(frame) - point.y);
}

}  // namespace

void AddMouseEventHandlerToClass(Class cl) {
  if ([cl instancesRespondToSelector:@selector(nativeMouseHandlerInjected)])
    return;
  class_addMethod(cl, @selector(nativeMouseHandlerInjected), (IMP)NativeDummy,
                  "B@:");

  class_addMethod(cl, @selector(mouseDown:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(rightMouseDown:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(otherMouseDown:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(mouseUp:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(rightMouseUp:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(otherMouseUp:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(mouseMoved:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(mouseDragged:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(rightMouseDragged:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(otherMouseDragged:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(mouseEntered:), (IMP)OnMouseEvent, "v@:@");
  class_addMethod(cl, @selector(mouseExited:), (IMP)OnMouseEvent, "v@:@");
}

bool IsMouseEventHandlerAddedToClass(Class cl) {
  if ([cl instancesRespondToSelector:@selector(nativeMouseHandlerInjected)])
    return true;
  return false;
}

bool DispatchMouseEvent(api::BaseView* view, NSEvent* event) {
  bool prevent_default = false;
  NativeViewPrivate* priv = [view->GetNSView() nativeViewPrivate];
  api::BaseView::MouseEventType type = MouseEventTypeFromNS(event);
  uint32_t timestamp = [event timestamp] * 1000;
  int button = [event buttonNumber] + 1;
  gfx::Point position_in_view = GetPosInView(event, view->GetNSView());
  gfx::Point position_in_window = GetPosInWindow(event, view->GetNSView());
  switch (type) {
    case api::BaseView::MouseEventType::kDown:
    case api::BaseView::MouseEventType::kUp:
      prevent_default = view->NotifyMouseEvent(
          type, timestamp, button, position_in_view, position_in_window);
      break;
    case api::BaseView::MouseEventType::kMove:
      view->NotifyMouseEvent(type, timestamp, button, position_in_view,
                             position_in_window);
      prevent_default = true;
      break;
    case api::BaseView::MouseEventType::kEnter:
      priv->hovered = true;
      view->NotifyMouseEvent(type, timestamp, button, position_in_view,
                             position_in_window);
      prevent_default = true;
      break;
    case api::BaseView::MouseEventType::kLeave:
      priv->hovered = false;
      view->NotifyMouseEvent(type, timestamp, button, position_in_view,
                             position_in_window);
      prevent_default = true;
      break;
    default:
      break;
  }
  return prevent_default;
}

}  // namespace electron
