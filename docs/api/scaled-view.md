# ScaledView

A `ScaledView` is a rectangle within the views View hierarchy.
It can be used to embed additional views hierarchy into a
[`BrowserWindow`](browser-window.md).
It extends [`BaseView`](base-view.md).

## Class: ScaledView extends `BaseView`

> Create and control scaled views.

Process: [Main](../glossary.md#main-process)

### `new ScaledView([options])` _Experimental_

* `options` Object (optional)
  * `vibrant` boolean (optional) - Enables the vibrant visual effect. Default is `false`.
  * `blurred` boolean (optional) - Enables the blurred visual effect. Default is `false`.

Creates the new scaled view.

### Static Methods

The `ScaledView` class has the following static methods:

#### `ScaledView.getAllViews()`

Returns `ScaledView[]` - An array of all created scaled views.

#### `ScaledView.fromId(id)`

* `id` Integer

Returns `ScaledView | null` - The scaled view with the given `id`.

### Instance Methods

Objects created with `new ScaledView` have the following instance methods:

  #### `view.getZoomFactor()` _Experimental_

Returns `Float` - scaling factor used for zooming in or zooming out.
