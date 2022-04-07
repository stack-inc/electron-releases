# BaseView

A `BaseView` is a rectangle within the views View hierarchy. It is the base
class for [`ContainerView`](container-view.md), [`ScrollView`](scroll-view.md).

## Class: BaseView

> The base view for all different views.

Process: [Main](../glossary.md#main-process)

`BaseView` is an [EventEmitter][event-emitter].

### `new BaseView()` _Experimental_

Creates the new base view.

### Instance Events

Objects created with `new BaseView` emit the following events:

#### Event: 'size-changed' _Experimental_

Returns:

* `event` Event
* `oldSize` [Size](structures/size.md) - Size the view was before.
* `newSize` [Size](structures/size.md) - Size the view is being resized to.

Emitted when the view's size has been changed.

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

#### `view.isContainer` _Readonly_ _Experimental_

A `boolean` property that determines whether this view is (or inherits from) [`ContainerView`](container-view.md).

#### `view.zIndex` _Experimental_

A `Integer` representing z-index to set. Has no effect until parent's `view.rearrangeChildViews()` is called.

#### `view.clickThrough` _macOS_ _Experimental_

A `Boolean` representing whether this BaseView receives any mouse input. Any BaseView
with `clickThrough = true` will still display but won't receive any mouse events.

### Instance Methods

Objects created with `new BaseView` have the following instance methods:

#### `view.setBounds(bounds[, options])` _Experimental_

* `bounds` [Rectangle](structures/rectangle.md) - The position and size of the view, relative to its parent.
* `options` Object (optional)
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

* `visible` boolean - Sets whether this view is visible.

#### `view.isVisible()` _Experimental_

Returns `boolean` - Whether the view is visible.

#### `view.IsTreeVisible()` _Experimental_

Returns `boolean` - Whether the view and its parent are visible.

#### `view.focus()` _Experimental_

Move the keyboard focus to the view.

#### `view.hasFocus()` _Experimental_

Returns `boolean` - Whether this view currently has the focus.

#### `view.setFocusable(focusable)` _Experimental_

* `focusable` boolean - Sets whether this view can be focused on.

#### `view.isFocusable()` _Experimental_

Returns `boolean` - Returns `true` if this view is focusable, enabled and drawn.

#### `view.setBackgroundColor(color)` _Experimental_

* `color` string - Color in `#aarrggbb` or `#argb` form. The alpha channel is
  optional.

Change the background color of the view.

#### `view.setRoundedCorners(options)` _macOS_ _Experimental_

* `options` Object
  * `radius` Number - Radius of each corner that is rounded.
  * `topLeft` Boolean (optional) - If `true`, top left corner will be rounded with `radius` value. `false` by default.
  * `topRight` Boolean (optional) - If `true`, top right corner will be rounded with `radius` value. `false` by default.
  * `bottomLeft` Boolean (optional) - If `true`, bottom left corner will be rounded with `radius` value. `false` by default.
  * `bottomRight` Boolean (optional) - If `true`, bottom right corner will be rounded with `radius` value. `false` by default.

#### `view.setClippingInsets(options)` _macOS_ _Experimental_

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

#### `view.getParentView()` _Experimental_

Returns `BaseView || null` - The parent view, otherwise returns `null`.

#### `view.getParentWindow()` _Experimental_

Returns `BrowserWindow || null` - The window that the view belongs to, otherwise returns `null`.

Note: The view can belongs  to either a view or a window. 
