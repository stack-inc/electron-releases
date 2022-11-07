// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_ELECTRON_INSPECTABLE_WEB_CONTENTS_VIEW_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_ELECTRON_INSPECTABLE_WEB_CONTENTS_VIEW_H_

#import <AppKit/AppKit.h>

#include "base/mac/scoped_nsobject.h"
#include "chrome/browser/devtools/devtools_contents_resizing_strategy.h"
#include "ui/base/cocoa/base_view.h"

namespace electron {
class InspectableWebContentsViewMac;
}

using electron::InspectableWebContentsViewMac;

@interface NSView (WebContentsView)
- (void)setMouseDownCanMoveWindow:(BOOL)can_move;
@end

@interface ControlRegionView : NSView
@end

@interface ElectronInspectableWebContentsView : BaseView <NSWindowDelegate> {
 @private
  electron::InspectableWebContentsViewMac* inspectableWebContentsView_;

  base::scoped_nsobject<NSView> fake_view_;
  base::scoped_nsobject<NSWindow> devtools_window_;
  base::scoped_nsobject<ControlRegionView> devtools_mask_;
  BOOL devtools_visible_;
  BOOL devtools_docked_;
  BOOL devtools_is_first_responder_;
  BOOL attached_to_window_;
  base::scoped_nsobject<NSImageView> thumbnail_;
  BOOL thumbnail_visible_;

  DevToolsContentsResizingStrategy strategy_;

  // Keep a reference to the filters for later modification
  CIFilter *_blurFilter, *_saturationFilter;
  CALayer* _hostedLayer;
}

- (instancetype)initWithInspectableWebContentsViewMac:
    (InspectableWebContentsViewMac*)view;
- (void)notifyDevToolsFocused;
- (void)setDevToolsVisible:(BOOL)visible activate:(BOOL)activate;
- (BOOL)isDevToolsVisible;
- (BOOL)isDevToolsFocused;
- (void)setIsDocked:(BOOL)docked activate:(BOOL)activate;
- (void)setContentsResizingStrategy:
    (const DevToolsContentsResizingStrategy&)strategy;
- (void)setTitle:(NSString*)title;
- (void)showThumbnail:(NSImage*)thumbnail;
- (void)hideThumbnail;

- (void)setTintColor:(NSColor*)color;
- (void)setBlurRadius:(float)radius;
- (float)blurRadius;
- (void)setSaturationFactor:(float)factor;
- (float)saturationFactor;

// The layer will be tinted using the tint color. By default it is a 70% White
// Color.
@property(strong, nonatomic) NSColor* _tintColor;

//* To get more vibrant colors, a filter to increase the saturation of the
// colors can be applied. The default value is 2.5.
@property(assign, nonatomic) float _saturationFactor;

//* The blur radius defines the strength of the Gaussian Blur filter. The
// default value is 20.0.
@property(assign, nonatomic) float _blurRadius;

@end

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_ELECTRON_INSPECTABLE_WEB_CONTENTS_VIEW_H_
