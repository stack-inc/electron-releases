const { app, BrowserView, BaseWindow, BrowserWindow, ContainerView, WrapperBrowserView } = require('electron')

function finishSettingBounds(view) {
  bounds = view.getBounds()
  console.log("bounds after animation: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
}

function settingBounds(view) {
  view.setBounds({ x: 600, y: 300, width: 600, height: 600 },
                 {"duration": 5, "timingControlPoints": {"x1": 0.47, "y1": 0, "x2": 0.745, "y2": 0.715}})
  setTimeout(finishSettingBounds, 6000, view)
}

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })

  const webContentView = new ContainerView()
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 400, y: 200, width: 650, height: 650})
  win.addChildView(webContentView)

  const browserView = new BrowserView()
  browserView.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView = new WrapperBrowserView({ 'browserView': browserView })
  wrapperBrowserView.setBounds({x: 0, y: 0, width: 600, height: 600})
  webContentView.addChildView(wrapperBrowserView)

  let bounds = webContentView.getBounds()
  console.log("bounds: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")

  browserView.webContents.on('did-finish-load', () => {
    setTimeout(settingBounds, 2000, webContentView)
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
