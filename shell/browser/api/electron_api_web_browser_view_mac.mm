// Copyright (c) 2022 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_web_browser_view.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#include <objc/objc-runtime.h>

#include "content/browser/web_contents/web_contents_impl.h"  // nogncheck
#include "shell/browser/api/electron_api_web_contents.h"
#import "shell/browser/ui/cocoa/electron_inspectable_web_contents_view.h"
#include "shell/browser/ui/cocoa/electron_native_view.h"
#include "shell/browser/ui/inspectable_web_contents.h"
#include "shell/browser/ui/inspectable_web_contents_view.h"
#include "ui/gfx/geometry/rect.h"

// Match view::Views behavior where the view sticks to the top-left origin.
const NSAutoresizingMaskOptions kDefaultAutoResizingMask =
    NSViewMaxXMargin | NSViewMinYMargin;

namespace electron {

namespace api {

#define kRMBlurredViewDefaultTintColor \
  [NSColor colorWithCalibratedWhite:1.0 alpha:0.7]
#define kRMBlurredViewDefaultSaturationFactor 2.0
#define kRMBlurredViewDefaultBlurRadius 20.0

void setTintColor(NSView* nsview, NSColor* color) {
  // Since we need a CGColor reference, store it for the drawing of the layer.
  if (color) {
    [nsview.layer setBackgroundColor:color.CGColor];
  }

  // Trigger a re-drawing of the layer
  [nsview.layer setNeedsDisplay];
}

void WebBrowserView::SetBlurTintColorWithSRGB(float r,
                                              float g,
                                              float b,
                                              float a) {
  BaseView::SetBlurTintColorWithSRGB(r, g, b, a);
  if (!IsBlurred())
    return;
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* iwc_nsview = iwc_view->GetNativeView().GetNativeNSView();
  NSColor* color = [NSColor colorWithSRGBRed:r green:g blue:b alpha:a];
  [static_cast<ElectronInspectableWebContentsView*>(iwc_nsview)
      setTintColor:color];

  auto* web_contents = api_web_contents_->GetWebContents();
  NSView* web_nsview = web_contents->GetNativeView().GetNativeNSView();
  setTintColor(web_nsview, color);
}

void WebBrowserView::SetBlurTintColorWithCalibratedWhite(float white,
                                                         float alphaval) {
  BaseView::SetBlurTintColorWithCalibratedWhite(white, alphaval);
  if (!IsBlurred())
    return;
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* iwc_nsview = iwc_view->GetNativeView().GetNativeNSView();
  NSColor* color = [NSColor colorWithCalibratedWhite:white alpha:alphaval];
  [static_cast<ElectronInspectableWebContentsView*>(iwc_nsview)
      setTintColor:color];

  auto* web_contents = api_web_contents_->GetWebContents();
  NSView* web_nsview = web_contents->GetNativeView().GetNativeNSView();
  setTintColor(web_nsview, color);
}

void WebBrowserView::SetBlurTintColorWithGenericGamma22White(float white,
                                                             float alphaval) {
  BaseView::SetBlurTintColorWithGenericGamma22White(white, alphaval);
  if (!IsBlurred())
    return;
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* iwc_nsview = iwc_view->GetNativeView().GetNativeNSView();
  NSColor* color = [NSColor colorWithGenericGamma22White:white alpha:alphaval];
  [static_cast<ElectronInspectableWebContentsView*>(iwc_nsview)
      setTintColor:color];

  auto* web_contents = api_web_contents_->GetWebContents();
  NSView* web_nsview = web_contents->GetNativeView().GetNativeNSView();
  setTintColor(web_nsview, color);
}

void WebBrowserView::SetBlurRadius(float radius) {
  BaseView::SetBlurRadius(radius);
  if (!IsBlurred())
    return;
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* iwc_nsview = iwc_view->GetNativeView().GetNativeNSView();
  [static_cast<ElectronInspectableWebContentsView*>(iwc_nsview)
      setBlurRadius:radius];

  _blurRadius = radius;
  resetFilters();
}

void WebBrowserView::SetBlurSaturationFactor(float factor) {
  BaseView::SetBlurSaturationFactor(factor);
  if (!IsBlurred())
    return;
  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;
  NSView* iwc_nsview = iwc_view->GetNativeView().GetNativeNSView();
  [static_cast<ElectronInspectableWebContentsView*>(iwc_nsview)
      setSaturationFactor:factor];

  _saturationFactor = factor;
  resetFilters();
}

void WebBrowserView::CreateWebBrowserView(
    InspectableWebContents* inspectable_web_contents) {
  if (!IsBlurred())
    SetView([[ElectronNativeView alloc] init]);
  else
    SetView([[ElectronNativeBlurredView alloc] init]);

  InspectableWebContentsView* iwc_view =
      inspectable_web_contents ? inspectable_web_contents->GetView() : nullptr;
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.autoresizingMask = kDefaultAutoResizingMask;
  [GetNSView() addSubview:view];

#if 0
  auto* web_contents = inspectable_web_contents->GetWebContents();
  NSView* web_nsview = web_contents->GetNativeView().GetNativeNSView();

  // Instantiate a new CALayer and set it as the NSView's layer (layer-hosting)
  CALayer* _hostedLayer = [CALayer layer];
  [web_nsview setWantsLayer:YES];
  [web_nsview setLayer:_hostedLayer];

  // Set up the default parameters
  _blurRadius = kRMBlurredViewDefaultBlurRadius;
  _saturationFactor = kRMBlurredViewDefaultSaturationFactor;
  setTintColor(web_nsview, kRMBlurredViewDefaultTintColor);

  // It's important to set the layer to mask to its bounds, otherwise the whole
  // parent view might get blurred
  [web_nsview.layer setMasksToBounds:YES];

  // To apply CIFilters on OS X 10.9, we need to set the property accordingly:
  if ([web_nsview
          respondsToSelector:@selector(setLayerUsesCoreImageFilters:)]) {
    BOOL flag = YES;
    NSInvocation* inv = [NSInvocation
        invocationWithMethodSignature:[web_nsview
                                          methodSignatureForSelector:@selector
                                          (setLayerUsesCoreImageFilters:)]];
    [inv setSelector:@selector(setLayerUsesCoreImageFilters:)];
    [inv setArgument:&flag atIndex:2];
    [inv invokeWithTarget:web_nsview];
  }

  // Set the layer to redraw itself once it's size is changed
  [web_nsview.layer setNeedsDisplayOnBoundsChange:YES];

  // Initially create the filter instances
  resetFilters();
#endif
}

void WebBrowserView::SetBounds(const gfx::Rect& bounds, gin::Arguments* args) {
  BaseView::SetBounds(bounds, args);

  auto* iwc_view = GetInspectableWebContentsView();
  if (!iwc_view)
    return;

  auto* view = iwc_view->GetNativeView().GetNativeNSView();
  view.frame = NSMakeRect(0, 0, bounds.width(), bounds.height());

  // Ensure draggable regions are properly updated to reflect new bounds.
  iwc_view->UpdateDraggableRegions(iwc_view->GetDraggableRegions());
}

void WebBrowserView::resetFilters() {
  auto* web_contents = api_web_contents_->GetWebContents();
  NSView* web_nsview = web_contents->GetNativeView().GetNativeNSView();

  // To get a higher color saturation, we create a ColorControls filter
  CIFilter* _saturationFilter = [CIFilter filterWithName:@"CIColorControls"];
  [_saturationFilter setDefaults];
  [_saturationFilter setValue:[NSNumber numberWithFloat:_saturationFactor]
                       forKey:@"inputSaturation"];

  // Next, we create the blur filter
  CIFilter* _blurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
  [_blurFilter setDefaults];
  [_blurFilter setValue:[NSNumber numberWithFloat:_blurRadius]
                 forKey:@"inputRadius"];

  // Now we apply the two filters as the layer's background filters
  [web_nsview.layer setBackgroundFilters:@[ _saturationFilter, _blurFilter ]];

  // ... and trigger a refresh
  [web_nsview.layer setNeedsDisplay];
}

}  // namespace api

}  // namespace electron
