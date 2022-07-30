const { app, BaseWindow, BrowserWindow, ContainerView, WebBrowserView } = require('electron')

const anchors = [
  {"posX": "30", "posY": "30"},
  {"posX": "left", "posY": "top"},
  {"posX": "center", "posY": "top"},
  {"posX": "right", "posY": "top"},
  {"posX": "left", "posY": "center"},
  {"posX": "center", "posY": "center"},
  {"posX": "right", "posY": "center"},
  {"posX": "left", "posY": "bottom"},
  {"posX": "center", "posY": "bottom"},
  {"posX": "right", "posY": "bottom"}
];
let anchorIndex = 0

function finishZoomIn(view) {
  bounds = view.getBounds()
  console.log("bounds after zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  const scaleX = view.getScaleX()
  const scaleY = view.getScaleY()
  console.log("scale after zooming in: (" + scaleX + ", " + scaleY + ")")

  if (anchorIndex < anchors.length) {
    view.setScale({"scaleX": 2.0, "scaleY": 2.0, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
    setTimeout(finishZoomOut, 6000, view)
  }
}

function finishZoomOut(view) {
  bounds = view.getBounds()
  console.log("bounds after zooming out: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  const scaleX = view.getScaleX()
  const scaleY = view.getScaleY()
  console.log("scale after zooming out: (" + scaleX + ", " + scaleY + ")")

  view.setScale({"scaleX": 1.0, "scaleY": 1.0, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
  anchorIndex++;
  setTimeout(finishZoomIn, 6000, view)
}

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })

  const webContentView = new ContainerView()
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 400, y: 500, width: 300, height: 300})
  win.addChildView(webContentView)

  const webBrowserView = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView.webContents.loadURL('https://electronjs.org')
  webBrowserView.setBounds({x: 0, y: 0, width: 300, height: 300})
  webContentView.addChildView(webBrowserView)

  let bounds = webBrowserView.getBounds()
  console.log("bounds: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")

  webBrowserView.webContents.on('did-finish-load', () => {
    webContentView.setScale({"scaleX": 2.0, "scaleY": 2.0, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
    setTimeout(finishZoomOut, 6000, webContentView)
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
