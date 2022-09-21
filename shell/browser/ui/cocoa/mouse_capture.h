#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_MOUSE_CAPTURE_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_MOUSE_CAPTURE_H_

#import <Cocoa/Cocoa.h>

namespace electron {

namespace api {
class BaseView;
}

// Simulating the SetCapture of Windows.
class MouseCapture {
 public:
  explicit MouseCapture(api::BaseView* view);
  ~MouseCapture();

  // disable copy
  MouseCapture(const MouseCapture&) = delete;
  MouseCapture& operator=(const MouseCapture&) = delete;

 private:
  void ProcessCapturedMouseEvent(NSEvent* event);

  api::BaseView* view_;
  id local_monitor_;
  id global_monitor_;
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_MOUSE_CAPTURE_H_
