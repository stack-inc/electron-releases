const { app, BaseView, BrowserView, BaseWindow, BrowserWindow, ContainerView, WrapperBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const webContentView = new ContainerView()
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 50, y: 50, width: 350, height: 350})
  win.addChildView(webContentView)

  const browserView = new BrowserView()
  browserView.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView = new WrapperBrowserView({ 'browserView': browserView })
  wrapperBrowserView.setBounds({x: 0, y: 0, width: 300, height: 300})
  webContentView.addChildView(wrapperBrowserView)

  const webContentView2 = new ContainerView()
  webContentView2.setBackgroundColor("#1F2937")
  webContentView2.setBounds({x: 500, y: 50, width: 350, height: 350})
  win.addChildView(webContentView2)
  webContentView2.setClippingInsets({ 'left': 40, 'top': 40, 'right': 40, 'bottom': 40 })

  const browserView2 = new BrowserView()
  browserView2.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView2 = new WrapperBrowserView({ 'browserView': browserView2 })
  wrapperBrowserView2.setBounds({x: 0, y: 0, width: 300, height: 300})
  webContentView2.addChildView(wrapperBrowserView2)
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
