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

bool DispatchMouseEvent(api::BaseView* view, NSEvent* event) {
  bool prevent_default = false;
  NativeViewPrivate* priv = [view->GetNSView() nativeViewPrivate];
  api::NativeMouseEvent mouse_event(event, view->GetNSView());
  switch (mouse_event.type) {
    case api::EventType::kLeftMouseDown:
    case api::EventType::kRightMouseDown:
    case api::EventType::kOtherMouseDown:
      prevent_default = view->NotifyMouseDown(mouse_event);
      break;
    case api::EventType::kLeftMouseUp:
    case api::EventType::kRightMouseUp:
    case api::EventType::kOtherMouseUp:
      prevent_default = view->NotifyMouseUp(mouse_event);
      break;
    case api::EventType::kMouseMove:
      view->NotifyMouseMove(mouse_event);
      prevent_default = true;
      break;
    case api::EventType::kMouseEnter:
      priv->hovered = true;
      view->NotifyMouseEnter(mouse_event);
      prevent_default = true;
      break;
    case api::EventType::kMouseLeave:
      priv->hovered = false;
      view->NotifyMouseLeave(mouse_event);
      prevent_default = true;
      break;
    default:
      break;
  }
  return prevent_default;
}

}  // namespace electron
