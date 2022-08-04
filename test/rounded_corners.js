const { app, BaseView, BaseWindow, BrowserWindow, WebBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const webContentView = new BaseView()
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 400, y: 200, width: 650, height: 650})
  win.addChildView(webContentView)

  const webBrowserView = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView.webContents.loadURL('https://electronjs.org')
  webBrowserView.setBounds({x: 0, y: 0, width: 600, height: 600})
  webContentView.addChildView(webBrowserView)
  webBrowserView.setRoundedCorners({ 'radius': 40, 'topLeft': true, 'topRight': true, 'bottomLeft': true, 'bottomRight': true })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
