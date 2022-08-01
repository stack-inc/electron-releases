const { app, BaseWindow, BrowserWindow, WebBrowserView } = require('electron')

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
  const win = new BaseWindow({ width: 1400, height: 1200 })

  const view = new WebBrowserView()
  win.addChildView(view)
  view.setBounds({ x: 400, y: 200, width: 600, height: 600 })
  view.webContents.loadURL('https://electronjs.org')
  let bounds = view.getBounds()
  console.log("bounds: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")

  view.webContents.on('did-finish-load', () => {
    setTimeout(settingBounds, 2000, view)
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
