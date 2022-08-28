#ifndef SHELL_BROWSER_UI_NATIVE_WEB_BROWSER_VIEW_H_
#define SHELL_BROWSER_UI_NATIVE_WEB_BROWSER_VIEW_H_

#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "shell/browser/ui/native_view.h"

namespace gfx {
class Image;
}

namespace electron {

class InspectableWebContents;
class InspectableWebContentsView;

class NativeWebBrowserView : public NativeView,
                             public content::WebContentsObserver {
 public:
  class Observer : public base::CheckedObserver {
   public:
    ~Observer() override {}

    virtual void OnSettingOwnerWindow(NativeWindow* window) {}
  };

  NativeWebBrowserView(InspectableWebContents* inspectable_web_contents);

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  InspectableWebContentsView* GetInspectableWebContentsView();
  void ShowThumbnail(gfx::Image thumbnail);
  void HideThumbnail();

  // content::WebContentsObserver:
  void WebContentsDestroyed() override;
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  void RenderViewReady() override;
#endif

  // NativeView:
#if BUILDFLAG(IS_MAC)
  void SetBounds(const gfx::Rect& bounds,
                 const BoundsAnimationOptions& options) override;
#endif
#if defined(TOOLKIT_VIEWS) && !BUILDFLAG(IS_MAC)
  void UpdateClickThrough() override;
  void SetRoundedCorners(const RoundedCornersOptions& options) override;
#endif

 protected:
  ~NativeWebBrowserView() override;

  // NativeView:
  void SetWindowForChildren(NativeWindow* window) override;

  void InitWebBrowserView();

 private:
  base::ObserverList<Observer> observers_;

  InspectableWebContents* inspectable_web_contents_;
};

}  // namespace electron

#endif  // SHELL_BROWSER_UI_NATIVE_WEB_BROWSER_VIEW_H_
