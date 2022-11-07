# WebBrowserView

A `WebBrowserView` can be used to embed additional web content into a
[`BrowserWindow`](browser-window.md).
It extends [`BaseView`](base-view.md).

## Class: WebBrowserView extends `BaseView`

> Create and control views.

Process: [Main](../glossary.md#main-process)

This module cannot be used until the `ready` event of the `app`
module is emitted.

### `new WebBrowserView([options])` _Experimental_

* `options` Object (optional)
  * `blurred` boolean (optional) - Enables the blurred visual effect. Default is `false`.
  * `webPreferences` [WebPreferences](structures/web-preferences.md) (optional) - Settings of web page's features.

### Instance Properties

Objects created with `new WebBrowserView` have the following properties:

#### `view.webContents` _Experimental_

A [`WebContents`](web-contents.md) object owned by this view.

### Instance Methods

Objects created with `new WebBrowserView` have the following instance methods:

#### `view.hide(freeze, thumbnail)` _Experimental_

* `freeze` boolean - Hides and freezes the page. A frozen page runs as few tasks as possible.
* `thumbnail` [NativeImage](native-image.md) - The thumbnail to show on top of the page.

#### `view.show()` _Experimental_

Hides the thumbnail. If the page is hidden and frozen, then it shows the page which is automatically unfrozen.
