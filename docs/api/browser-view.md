# BrowserView

A `BrowserView` can be used to embed additional web content into a
[`BrowserWindow`](browser-window.md). It is like a child window, except that it is positioned
relative to its owning window. It is meant to be an alternative to the
`webview` tag.

## Class: BrowserView

> Create and control views.

Process: [Main](../glossary.md#main-process)

This module cannot be used until the `ready` event of the `app`
module is emitted.

### Example

```javascript
// In the main process.
const { app, BrowserView, BrowserWindow } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 800, height: 600 })

  const view = new BrowserView()
  win.setBrowserView(view)
  view.setBounds({ x: 0, y: 0, width: 300, height: 300 })
  view.webContents.loadURL('https://electronjs.org')
})
```

### `new BrowserView([options])` _Experimental_

* `options` Object (optional)
  * `webPreferences` Object (optional) - See [BrowserWindow](browser-window.md).

### Instance Properties

Objects created with `new BrowserView` have the following properties:

#### `view.webContents` _Experimental_

A [`WebContents`](web-contents.md) object owned by this view.

#### `view.clickThrough` _macOS_ _Experimental_

A `Boolean` representing whether this BrowserView receives any mouse input. Any BrowserView
with `clickThrough = true` will still display but won't receive any mouse events.

### Instance Methods

Objects created with `new BrowserView` have the following instance methods:

#### `view.setAutoResize(options)` _Experimental_

* `options` Object
  * `width` boolean (optional) - If `true`, the view's width will grow and shrink together
    with the window. `false` by default.
  * `height` boolean (optional) - If `true`, the view's height will grow and shrink
    together with the window. `false` by default.
  * `horizontal` boolean (optional) - If `true`, the view's x position and width will grow
    and shrink proportionally with the window. `false` by default.
  * `vertical` boolean (optional) - If `true`, the view's y position and height will grow
    and shrink proportionally with the window. `false` by default.

#### `view.setBounds(bounds[, options])` _Experimental_

* `bounds` [Rectangle](structures/rectangle.md)
* `options` Object (optional)
  * `duration` Float (optional) - A duration of the animation (in seconds). Default is 1.0.
  * `timingFunction` string (optional) - One of the following values: `linear`, `easeIn`, `easeOut`, `easeInEaseOut`, `default`. Default is `linear`.
  * `timingControlPoints` Object (optional)
    * `x1` Float (optional) - A floating point number representing the x position of the c1 control point.
    * `y1` Float (optional) - A floating point number representing the y position of the c1 control point.
    * `x2` Float (optional) - A floating point number representing the x position of the c2 control point.
    * `y2` Float (optional) - A floating point number representing the y position of the c2 control point.

Resizes and moves the view to the supplied bounds relative to the window.
In case one of the values : `duration`, `timingFunction`, `timingControlPoints` is specified, the resizing / moving is animated.

It's possible to specify a function that defines the pacing of an animation as a timing curve. 
The function maps an input time normalized to the range [0,1] to an output time also in the range [0,1].
You can specify a media timing function by supplying your own cubic Bézier curve control points using the `timingControlPoints` options or by using one of the predefined timing functions.

You can configure the timing function with the predefined timing function specified by name:
* `linear` - Linear pacing, which causes an animation to occur evenly over its duration
* `easeIn` - Ease-in pacing, which causes an animation to begin slowly and then speed up as it progresses
* `easeOut` - Ease-out pacing, which causes an animation to begin quickly and then slow as it progresses
* `easeInEaseOut` - Ease-in-ease-out pacing, which causes an animation to begin slowly, accelerate through the middle of its duration, and then slow again before completing
* `default` - Similar to 3t^2 - 2t^3, one of the basic Hermite blending functions.

When you supply your own cubic Bézier curve control points, then the function is modeled as a cubic Bézier curve.
The end points of the Bézier curve are automatically set to (0.0,0.0) and (1.0,1.0). The control points defining the Bézier curve are: [(0.0,0.0), (c1x,c1y), (c2x,c2y), (1.0,1.0)].

#### `view.getBounds()` _Experimental_

Returns [`Rectangle`](structures/rectangle.md)

The `bounds` of this BrowserView instance as `Object`.

#### `view.setBackgroundColor(color)` _Experimental_

* `color` string - Color in Hex, RGB, ARGB, HSL, HSLA or named CSS color format. The alpha channel is
  optional for the hex type.

Examples of valid `color` values:

* Hex
  * #fff (RGB)
  * #ffff (ARGB)
  * #ffffff (RRGGBB)
  * #ffffffff (AARRGGBB)
* RGB
  * rgb\((\[\d]+),\s*(\[\d]+),\s*(\[\d]+)\)
    * e.g. rgb(255, 255, 255)
* RGBA
  * rgba\((\[\d]+),\s*(\[\d]+),\s*(\[\d]+),\s*(\[\d.]+)\)
    * e.g. rgba(255, 255, 255, 1.0)
* HSL
  * hsl\((-?\[\d.]+),\s*(\[\d.]+)%,\s*(\[\d.]+)%\)
    * e.g. hsl(200, 20%, 50%)
* HSLA
  * hsla\((-?\[\d.]+),\s*(\[\d.]+)%,\s*(\[\d.]+)%,\s*(\[\d.]+)\)
    * e.g. hsla(200, 20%, 50%, 0.5)
* Color name
  * Options are listed in [SkParseColor.cpp](https://source.chromium.org/chromium/chromium/src/+/main:third_party/skia/src/utils/SkParseColor.cpp;l=11-152;drc=eea4bf52cb0d55e2a39c828b017c80a5ee054148)
  * Similar to CSS Color Module Level 3 keywords, but case-sensitive.
    * e.g. `blueviolet` or `red`

**Note:** Hex format with alpha takes `AARRGGBB` or `ARGB`, _not_ `RRGGBBA` or `RGA`.

#### `view.resetScaling()` _Experimental_

Makes the scaling of the view equal to the window's base coordinate system.

#### `view.setScale(options)` _Experimental_

* `options` Object
  * `scaleX` number (optional) - Specifies the x value for the new unit size.
  * `scaleY` number (optional) - Specifies the y value for the new unit size.
  * `adjustFrame` string (optional) - Adjusts the bounds (frame) to the scaled bounds.
  * `anchorX` string (optional) - The x coordinate of the anchor point when scaling. It can be one of the following values: `left`, `center`, `right`. It can be also the number between 1 and 100 (specifying the percentage).
  * `anchorY` string (optional) - The y coordinate of the anchor point when scaling. It can be one of the following values: `top`, `center`, `bottom`. It can be also the number between 1 and 100 (specifying the percentage).
  * `animation` Object (optional)
    * `duration` Float (optional) - A duration of the animation (in seconds). Default is 1.0.
    * `timingFunction` string (optional) - One of the following values: `linear`, `easeIn`, `easeOut`, `easeInEaseOut`, `default`. Default is `linear`.
    * `timingControlPoints` Object (optional)
      * `x1` Float (optional) - A floating point number representing the x position of the c1 control point.
      * `y1` Float (optional) - A floating point number representing the y position of the c1 control point.
      * `x2` Float (optional) - A floating point number representing the x position of the c2 control point.
      * `y2` Float (optional) - A floating point number representing the y position of the c2 control point.

Scales the view’s coordinate system so that the unit square scales to the specified dimensions.
In case when `animation` is specified, the scaling and resizing / moving is animated.
The function does the simultaneous translation animation to keep the view's anchor during the zoom animation.

It's possible to specify a function that defines the pacing of an animation as a timing curve. 
The function maps an input time normalized to the range [0,1] to an output time also in the range [0,1].
You can specify a media timing function by supplying your own cubic Bézier curve control points using the `timingControlPoints` options or by using one of the predefined timing functions.

You can configure the timing function with the predefined timing function specified by name:
* `linear` - Linear pacing, which causes an animation to occur evenly over its duration
* `easeIn` - Ease-in pacing, which causes an animation to begin slowly and then speed up as it progresses
* `easeOut` - Ease-out pacing, which causes an animation to begin quickly and then slow as it progresses
* `easeInEaseOut` - Ease-in-ease-out pacing, which causes an animation to begin slowly, accelerate through the middle of its duration, and then slow again before completing
* `default` - Similar to 3t^2 - 2t^3, one of the basic Hermite blending functions.

When you supply your own cubic Bézier curve control points, then the function is modeled as a cubic Bézier curve.
The end points of the Bézier curve are automatically set to (0.0,0.0) and (1.0,1.0). The control points defining the Bézier curve are: [(0.0,0.0), (c1x,c1y), (c2x,c2y), (1.0,1.0)].

#### `view.getScaleX()` _Experimental_

Returns the unit square scale of x dimension.
 
#### `view.getScaleY()` _Experimental_

Returns the unit square scale of x dimension.

#### `view.setOpacity(opacity[, options])` _Experimental_

* `opacity` number - between 0.0 (fully transparent) and 1.0 (fully opaque)
* `options` Object (optional)
  * `duration` Float (optional) - A duration of the animation (in seconds). Default is 1.0.
  * `timingFunction` string (optional) - One of the following values: `linear`, `easeIn`, `easeOut`, `easeInEaseOut`, `default`. Default is `linear`.
  * `timingControlPoints` Object (optional)
    * `x1` Float (optional) - A floating point number representing the x position of the c1 control point.
    * `y1` Float (optional) - A floating point number representing the y position of the c1 control point.
    * `x2` Float (optional) - A floating point number representing the x position of the c2 control point.
    * `y2` Float (optional) - A floating point number representing the y position of the c2 control point.

Sets the opacity of the view. Out of bound number values are clamped to the [0, 1] range.
In case one of the values : `duration`, `timingFunction`, `timingControlPoints` is specified, the opacity change is animated.

#### `view.getOpacity()` _Experimental_

Returns `number` - between 0.0 (fully transparent) and 1.0 (fully opaque).

#### `view.setVisible(visible)` _Experimental_

* `visible` boolean - Sets whether this view is visible.

#### `view.isVisible()` _Experimental_

Returns `boolean` - Whether the view is visible.

#### `view.hide(freeze, thumbnail)` _Experimental_

* `freeze` boolean - Hides and freezes the page. A frozen page runs as few tasks as possible.
* `thumbnail` [NativeImage](native-image.md) - The thumbnail to show on top of the page.

#### `view.show()` _Experimental_

Hides the thumbnail. If the page is hidden and frozen, then it shows the page which is automatically unfrozen.
