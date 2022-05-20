#ifndef SHELL_BROWSER_UI_VIEW_UTILS_H_
#define SHELL_BROWSER_UI_VIEW_UTILS_H_

#include "build/build_config.h"

namespace gin_helper {
class Dictionary;
}

namespace gfx {
class Rect;
}

#if BUILDFLAG(IS_MAC)
#ifdef __OBJC__
@class NSView;
#else
struct NSView;
#endif
#endif

namespace electron {

#if BUILDFLAG(IS_MAC)
void SetBoundsForView(NSView* view,
                      const gfx::Rect& bounds,
                      const gin_helper::Dictionary& options);

// Makes the scaling of the receiver equal to the window's
// base coordinate system.
void ResetScalingForView(NSView* view);
void SetScaleForView(NSView* view, const gin_helper::Dictionary& options);
float GetScaleXForView(NSView* view);
float GetScaleYForView(NSView* view);
void SetOpacityForView(NSView* view,
                       const double opacity,
                       const gin_helper::Dictionary& options);
double GetOpacityForView(NSView* view);
#endif

}  // namespace electron

#endif  // SHELL_BROWSER_UI_VIEW_UTILS_H_
