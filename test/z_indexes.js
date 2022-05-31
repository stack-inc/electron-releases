const { app, BaseView, BrowserView, BaseWindow, BrowserWindow, ContainerView, WrapperBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const contents = new ContainerView()
  contents.setBounds({x: 0, y: 0, width: 1300, height: 1100})
  win.addChildView(contents)

  const webContentView = new ContainerView()
  webContentView.zIndex = 2;
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 50, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView)
  const browserView = new BrowserView()
  browserView.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView = new WrapperBrowserView({ 'browserView': browserView })
  wrapperBrowserView.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView.addChildView(wrapperBrowserView)

  const webContentView2 = new ContainerView()
  webContentView2.zIndex = 4;
  webContentView2.setBackgroundColor("#1F2937")
  webContentView2.setBounds({x: 350, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView2)
  const browserView2 = new BrowserView()
  browserView2.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView2 = new WrapperBrowserView({ 'browserView': browserView2 })
  wrapperBrowserView2.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView2.addChildView(wrapperBrowserView2)

  const webContentView3 = new ContainerView()
  webContentView3.zIndex = 1;
  webContentView3.setBackgroundColor("#1F2937")
  webContentView3.setBounds({x: 650, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView3)
  const browserView3 = new BrowserView()
  browserView3.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView3 = new WrapperBrowserView({ 'browserView': browserView3 })
  wrapperBrowserView3.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView3.addChildView(wrapperBrowserView3)

  const webContentView4 = new ContainerView()
  webContentView4.zIndex = 3;
  webContentView4.setBackgroundColor("#1F2937")
  webContentView4.setBounds({x: 950, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView4)
  const browserView4 = new BrowserView()
  browserView4.webContents.loadURL('https://electronjs.org')
  const wrapperBrowserView4 = new WrapperBrowserView({ 'browserView': browserView4 })
  wrapperBrowserView4.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView4.addChildView(wrapperBrowserView4)

  contents.rearrangeChildViews()
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
