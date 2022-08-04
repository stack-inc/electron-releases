const { app, BaseView, BaseWindow, BrowserWindow, WebBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const webContentView = new BaseView()
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 50, y: 50, width: 350, height: 350})
  win.addChildView(webContentView)

  const webBrowserView = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView.webContents.loadURL('https://electronjs.org')
  webBrowserView.setBounds({x: 0, y: 0, width: 300, height: 300})
  webContentView.addChildView(webBrowserView)

  const webContentView2 = new BaseView()
  webContentView2.setBackgroundColor("#1F2937")
  webContentView2.setBounds({x: 500, y: 50, width: 350, height: 350})
  win.addChildView(webContentView2)
  webContentView2.setClippingInsets({ 'left': 40, 'top': 40, 'right': 40, 'bottom': 40 })

  const webBrowserView2 = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView2.webContents.loadURL('https://electronjs.org')
  webBrowserView2.setBounds({x: 0, y: 0, width: 300, height: 300})
  webContentView2.addChildView(webBrowserView2)
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
