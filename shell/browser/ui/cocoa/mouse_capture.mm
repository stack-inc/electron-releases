#include "shell/browser/ui/cocoa/mouse_capture.h"

#include "shell/browser/ui/cocoa/events_handler.h"
#include "shell/browser/ui/native_view.h"

namespace electron {

MouseCapture::MouseCapture(NativeView* view) : view_(view) {
  NSEventMask event_mask = NSEventMaskLeftMouseDown | NSEventMaskLeftMouseUp |
                           NSEventMaskRightMouseDown | NSEventMaskRightMouseUp |
                           NSEventMaskMouseMoved | NSEventMaskLeftMouseDragged |
                           NSEventMaskRightMouseDragged |
                           NSEventMaskMouseEntered | NSEventMaskMouseExited |
                           NSEventMaskScrollWheel | NSEventMaskOtherMouseDown |
                           NSEventMaskOtherMouseUp |
                           NSEventMaskOtherMouseDragged;
  local_monitor_ = [NSEvent
      addLocalMonitorForEventsMatchingMask:event_mask
                                   handler:^NSEvent*(NSEvent* event) {
                                     ProcessCapturedMouseEvent(event);
                                     return nil;  // Swallow all local events.
                                   }];
  global_monitor_ =
      [NSEvent addGlobalMonitorForEventsMatchingMask:event_mask
                                             handler:^void(NSEvent* event) {
                                               ProcessCapturedMouseEvent(event);
                                             }];
}

MouseCapture::~MouseCapture() {
  [NSEvent removeMonitor:global_monitor_];
  [NSEvent removeMonitor:local_monitor_];
}

void MouseCapture::ProcessCapturedMouseEvent(NSEvent* event) {
  DispatchMouseEvent(view_, event);
}

}  // namespace electron
