const { app, BrowserView, BaseWindow, BrowserWindow } = require('electron')

function finishFadeIn(view) {
  console.log("finish fade in - opacity: " + view.getOpacity())
}

function finishFadeOut(view) {
  console.log("finish fade out - opacity: " + view.getOpacity())
  view.setOpacity(1.0,
                 {"duration": 5, "timingControlPoints": {"x1": 0.47, "y1": 0, "x2": 0.745, "y2": 0.715}})
  setTimeout(finishFadeIn, 6000, view)
}

function startFadeOut(view) {
  console.log("start fade out - opacity: " + view.getOpacity())
  view.setOpacity(0.1,
                 {"duration": 5, "timingControlPoints": {"x1": 0.47, "y1": 0, "x2": 0.745, "y2": 0.715}})
  setTimeout(finishFadeOut, 6000, view)
}

app.whenReady().then(() => {
  const win = new BaseWindow({ width: 1400, height: 1200 })

  const view = new BrowserView()
  win.setBrowserView(view)
  view.setBounds({ x: 400, y: 200, width: 600, height: 600 })
  view.webContents.loadURL('https://electronjs.org')

  view.webContents.on('did-finish-load', () => {
    setTimeout(startFadeOut, 2000, view)
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
