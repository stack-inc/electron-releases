# BaseView

A `BaseView` is a rectangle within the views View hierarchy.
It can be used to embed additional views hierarchy into a
[`BrowserWindow`](browser-window.md).

## Class: BaseView

> The base view for all different views.

Process: [Main](../glossary.md#main-process)

`BaseView` is an [EventEmitter][event-emitter].

### `new BaseView([options])` _Experimental_

* `options` Object (optional)
  * `vibrant` boolean (optional) - Enables the vibrant visual effect. Default is `false`.
  * `blurred` boolean (optional) - Enables the blurred visual effect. Default is `false`.

Creates the new base view.

### Instance Events

Objects created with `new BaseView` emit the following events:

#### Event: 'size-changed' _Experimental_

Returns:

* `event` Event
* `oldSize` [Size](structures/size.md) - Size the view was before.
* `newSize` [Size](structures/size.md) - Size the view is being resized to.

Emitted when the view's size has been changed.

#### Event: 'mouse-down' _Experimental_

Returns:

* `event` Event
* `timestamp` Integer - Time when event was created, starts from when machine was booted.
* `button` Integer - The mouse button being pressed, `1` is for left button, `2` is for right button, `3` is for middle button.
* `positionInView` [Point](structures/point.md) - Relative position inside the view where the event happened.
* `positionInWindow` [Point](structures/point.md) - Relative position inside the window.

Emitted when pressing mouse buttons. Calling `event.preventDefault()` will prevent the view from receiving the mouse down event.

#### Event: 'mouse-up' _Experimental_

Returns:

* `event` Event
* `timestamp` Integer - Time when event was created, starts from when machine was booted.
* `button` Integer - The mouse button being pressed, `1` is for left button, `2` is for right button, `3` is for middle button.
* `positionInView` [Point](structures/point.md) - Relative position inside the view where the event happened.
* `positionInWindow` [Point](structures/point.md) - Relative position inside the window.

Emitted when releasing mouse buttons. Calling `event.preventDefault()` will prevent the view from receiving the mouse down event.

#### Event: 'mouse-move' _Experimental_

Returns:

* `event` Event
* `timestamp` Integer - Time when event was created, starts from when machine was booted.
* `button` Integer - The mouse button being pressed, `1` is for left button, `2` is for right button, `3` is for middle button.
* `positionInView` [Point](structures/point.md) - Relative position inside the view where the event happened.
* `positionInWindow` [Point](structures/point.md) - Relative position inside the window.

Emitted when user moves mouse in the view.

#### Event: 'mouse-enter' _Experimental_

Returns:

* `event` Event
* `timestamp` Integer - Time when event was created, starts from when machine was booted.
* `button` Integer - The mouse button being pressed, `1` is for left button, `2` is for right button, `3` is for middle button.
* `positionInView` [Point](structures/point.md) - Relative position inside the view where the event happened.
* `positionInWindow` [Point](structures/point.md) - Relative position inside the window.

Emitted when mouse enters the view.

#### Event: 'mouse-leave' _Experimental_

Returns:

* `event` Event
* `timestamp` Integer - Time when event was created, starts from when machine was booted.
* `button` Integer - The mouse button being pressed, `1` is for left button, `2` is for right button, `3` is for middle button.
* `positionInView` [Point](structures/point.md) - Relative position inside the view where the event happened.
* `positionInWindow` [Point](structures/point.md) - Relative position inside the window.

Emitted when mouse leaves the view.

#### Event: 'capture-lost' _macOS_ _Experimental_

Returns:

* `event` Event

Emitted when the mouse capture on view has been released.

### Static Methods

The `BaseView` class has the following static methods:

#### `BaseView.getAllViews()`

Returns `BaseView[]` - An array of all created views.

#### `BaseView.fromId(id)`

* `id` Integer

Returns `BaseView | null` - The view with the given `id`.

### Instance Properties

Objects created with `new BaseView` have the following properties:

#### `view.id` _Readonly_ _Experimental_

A `Integer` property representing the unique ID of the view. Each ID is unique among all `BaseView` instances of the entire Electron application.

#### `view.zIndex` _Experimental_

A `Integer` representing z-index to set. Has no effect until parent's `view.rearrangeChildViews()` is called.

#### `view.clickThrough` _Experimental_

A `Boolean` representing whether this BaseView receives any mouse input. Any BaseView
with `clickThrough = true` will still display but won't receive any mouse events.

### Instance Methods

Objects created with `new BaseView` have the following instance methods:

#### `view.setBounds(bounds[, options])` _Experimental_

* `bounds` [Rectangle](structures/rectangle.md) - The target position and size of the view, relative to its parent.
* `options` Object (optional)
  * `fromBounds` [Rectangle](structures/rectangle.md) (optional) - The initial position and size of the view, relative to its parent.
  * `duration` Float (optional) - A duration of the animation (in seconds). Default is 1.0.
  * `timingFunction` string (optional) - One of the following values: `linear`, `easeIn`, `easeOut`, `easeInEaseOut`, `default`. Default is `linear`.
  * `timingControlPoints` Object (optional)
    * `x1` Float (optional) - A floating point number representing the x position of the c1 control point.
    * `y1` Float (optional) - A floating point number representing the y position of the c1 control point.
    * `x2` Float (optional) - A floating point number representing the x position of the c2 control point.
    * `y2` Float (optional) - A floating point number representing the y position of the c2 control point.

Resizes and moves the view to the supplied bounds relative to its parent.
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

Returns the position and size of the view, relative to its parent.

#### `view.offsetFromView(from)` _Experimental_

* `from` `BaseView`

Returns [`Point`](structures/point.md) - Offset from `from` view.

Converts `(0, 0)` point from the coordinate system of a given `from` to that of the view.
both `from` and the view must belong to the same [`BrowserWindow`](browser-window.md).

#### `view.offsetFromWindow()` _Experimental_

Returns [`Point`](structures/point.md) - Offset the window that owns the view.

Converts `(0, 0)` point from window base coordinates to that of the view.

#### `view.setVisible(visible)` _Experimental_

* `visible` boolean - Whether this view is visible. Default is `true`.

#### `view.isVisible()` _Experimental_

Returns `boolean` - Whether the view is visible.

#### `view.IsTreeVisible()` _Experimental_

Returns `boolean` - Whether the view and its parent are visible.

#### `view.focus()` _Experimental_

Move the keyboard focus to the view.

#### `view.hasFocus()` _Experimental_

Returns `boolean` - Whether this view currently has the focus.

#### `view.setFocusable(focusable)` _Experimental_

* `focusable` boolean - Whether this view can be focused on.

#### `view.isFocusable()` _Experimental_

Returns `boolean` - Returns `true` if this view is focusable, enabled and drawn.

#### `view.setBackgroundColor(color)` _Experimental_

* `color` string - Color in Hex, RGB, ARGB, HSL, HSLA or named CSS color format. The alpha channel is
  optional for the hex type.

Change the background color of the view.

#### `view.setVisualEffectMaterial(material)` _macOS_ _Experimental_

* `material` string - Can be `appearanceBased`, `light`, `dark`, `titlebar`. Default is `appearanceBased`.

Sets the material shown by the vibrant view.
* `appearanceBased` - The default material for the view’s effective appearance.
* `light` - The material with a light effect.
*`dark` - The material with a dark effect.
*`titlebar` - The material for a window’s titlebar.

This function works when `view` is created with `vibrant` option.

#### `view.getVisualEffectMaterial()` _macOS_ _Experimental_

Returns `string` - The material shown by the vibrant view.

This function works when `view` is created with `vibrant` option.

#### `view.setVisualEffectBlendingMode(mode)` _macOS_ _Experimental_

* `mode` string - Can be `behindWindow`, `withinWindow`. Default is `behindWindow`.

Sets whether the vibrant view blends with what's either behind or within the window.
* `behindWindow` - The mode that blends and blurs the vibrant view with the contents behind the window, such as the desktop or other windows.
* `withinWindow` - The mode that blends and blurs the vibrant view with contents behind the view in the current window only.

This function works when `view` is created with `vibrant` option.

#### `view.getVisualEffectBlendingMode()` _macOS_ _Experimental_

Returns `string` - Whether the vibrant view blends with what's either behind or within the window.

This function works when `view` is created with `vibrant` option.

#### `view.setBlurTintColorWithSRGB(red, green, blue, alpha)` _macOS_ _Experimental_

* `red` Float - The red component of the color .
* `green` Float - The green component of the color .
* `blue` Float - The blue component of the color .
* `alpha` Float - The opacity value of the color .

Sets a color for blurred view from the specified components in the sRGB colorspace. 
This function works when `view` is created with `blurred` option.

#### `view.setBlurTintColorWithCalibratedWhite(white, alpha)` _macOS_ _Experimental_

* `white` Float - The grayscale value of the color. Default is 1.0.
* `alpha` Float - The opacity value of the color . Default is 0.7.

Sets a color for blurred view using the given opacity and grayscale values.
The layer will be tinted using the tint color. By default it is a 70% White Color.
This function works when `view` is created with `blurred` option.

#### `view.setBlurTintColorWithGenericGamma22White(white, alpha)` _macOS_ _Experimental_

* `white` Float - The grayscale value of the color. Default is 1.0.
* `alpha` Float - The opacity value of the color . Default is 0.7.

Sets a color for blurred view with the specified white and alpha values in the GenericGamma22 colorspace.
This function works when `view` is created with `blurred` option.

#### `view.setBlurRadius(radius)` _macOS_ _Experimental_

* `radius` Float - The strength of the Gaussian Blur filter. Default is 20.

This function works when `view` is created with `blurred` option.

#### `view.getBlurRadius()` _macOS_ _Experimental_

Returns `Float` - The strength of the Gaussian Blur filter.

This function works when `view` is created with `blurred` option.

#### `view.setBlurSaturationFactor(factor)` _macOS_ _Experimental_

* `factor` Float - The saturation of the colors (To get more vibrant colors). Default is 2.0.

This function works when `view` is created with `blurred` option.

#### `view.getBlurSaturationFactor()` _macOS_ _Experimental_

Returns `Float` - The saturation factor.

This function works when `view` is created with `blurred` option.

#### `view.setCapture()` _macOS_ _Experimental_

Set mouse capture to the view.

#### `view.releaseCapture()` _macOS_ _Experimental_

Release mouse capture if the view has mouse capture.

#### `view.hasCapture()` _macOS_ _Experimental_

Returns `boolean` - Whether the view has mouse capture.

#### `view.enableMouseEvents()` _Experimental_

Enable mouse events `mouse-down` and `mouse-up` for a view and all its children.

**Note:** On Windows, you need to add event handlers separately for the given view and each of its children to handle the events for the entire hierarchy of the view.

#### `view.areMouseEventsEnabled()` _Experimental_

Returns `boolean` - Whether the mouse events are enabled.

#### `view.setMouseTrackingEnabled(enable)` _Experimental_

* `enable` boolean - Whether the mouse tracking is enabled. Default is `false`.

Enables/disables generating the mouse-tracking events: `mouse-move`, `mouse-enter`, `mouse-leave` for a view and all its children. Enabling tracking enables mouse events.

**Note:** On Windows, you need to add event handlers separately for the given view and each of its children to handle the events for the entire hierarchy of the view.

#### `view.isMouseTrackingEnabled()` _Experimental_

Returns `boolean` - Whether the mouse tracking is enabled.

#### `view.setRoundedCorners(options)` _Experimental_

* `options` Object
  * `radius` Number - Radius of each corner that is rounded.
  * `topLeft` Boolean (optional) - If `true`, top left corner will be rounded with `radius` value. `false` by default.
  * `topRight` Boolean (optional) - If `true`, top right corner will be rounded with `radius` value. `false` by default.
  * `bottomLeft` Boolean (optional) - If `true`, bottom left corner will be rounded with `radius` value. `false` by default.
  * `bottomRight` Boolean (optional) - If `true`, bottom right corner will be rounded with `radius` value. `false` by default.

#### `view.setClippingInsets(options)` _Experimental_

* `options` Object
  * `left` Number (optional) - How many pixels to clip from the left side of the browser view.
  * `top` Number (optional) - How many pixels to clip from the top of the browser view.
  * `right` Number (optional) - How many pixels to clip from the right side of the browser view.
  * `bottom` Number (optional) - How many pixels to clip from the bottom side of the browser view.

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

#### `view.addChildView(view)` _Experimental_

* `view` BaseView

#### `view.removeChildView(view)` _Experimental_

* `view` BaseView

#### `view.rearrangeChildViews()` _Experimental_

Rearranges child views according to their z-indexes.

#### `view.getViews()` _Experimental_

Returns `BaseView[]` - an array of all BaseViews that have been attached
with `addChildView`.

#### `view.getParentView()` _Experimental_

Returns `BaseView || null` - The parent view, otherwise returns `null`.

#### `view.getParentWindow()` _Experimental_

Returns `BrowserWindow || null` - The window that the view belongs to, otherwise returns `null`.

Note: The view can belongs  to either a view or a window. 
