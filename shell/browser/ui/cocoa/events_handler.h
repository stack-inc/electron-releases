#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_

#import <Cocoa/Cocoa.h>

namespace electron {

class NativeView;

// Dynamically add event methods to a NSView.
void AddMouseEventHandlerToClass(Class cl);

// Dispatch mouse events to a view.
bool DispatchMouseEvent(NativeView* view, NSEvent* event);

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_
