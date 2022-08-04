const { app, BaseView, BaseWindow, BrowserWindow, WebBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const contents = new BaseView()
  contents.setBounds({x: 0, y: 0, width: 1300, height: 1100})
  win.addChildView(contents)

  const webContentView = new BaseView()
  webContentView.zIndex = 2;
  webContentView.setBackgroundColor("#1F2937")
  webContentView.setBounds({x: 50, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView)
  const webBrowserView = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView.webContents.loadURL('https://electronjs.org')
  webBrowserView.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView.addChildView(webBrowserView)

  const webContentView2 = new BaseView()
  webContentView2.zIndex = 4;
  webContentView2.setBackgroundColor("#1F2937")
  webContentView2.setBounds({x: 350, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView2)
  const webBrowserView2 = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView2.webContents.loadURL('https://electronjs.org')
  webBrowserView2.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView2.addChildView(webBrowserView2)

  const webContentView3 = new BaseView()
  webContentView3.zIndex = 1;
  webContentView3.setBackgroundColor("#1F2937")
  webContentView3.setBounds({x: 650, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView3)
  const webBrowserView3 = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView3.webContents.loadURL('https://electronjs.org')
  webBrowserView3.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView3.addChildView(webBrowserView3)

  const webContentView4 = new BaseView()
  webContentView4.zIndex = 3;
  webContentView4.setBackgroundColor("#1F2937")
  webContentView4.setBounds({x: 950, y: 50, width: 250, height: 250})
  contents.addChildView(webContentView4)
  const webBrowserView4 = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView4.webContents.loadURL('https://electronjs.org')
  webBrowserView4.setBounds({x: 0, y: 0, width: 200, height: 200})
  webContentView4.addChildView(webBrowserView4)

  contents.rearrangeChildViews()
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
