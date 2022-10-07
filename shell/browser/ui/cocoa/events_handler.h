#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_

#import <Cocoa/Cocoa.h>

namespace electron {

namespace api {
class BaseView;
}

// Dynamically add event methods to a NSView.
void AddMouseEventHandlerToClass(Class cl);

bool IsMouseEventHandlerAddedToClass(Class cl);

// Dispatch mouse events to a view.
bool DispatchMouseEvent(api::BaseView* view, NSEvent* event);

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_EVENTS_HANDLER_H_
