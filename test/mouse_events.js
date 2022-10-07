const { app, BaseView, BaseWindow, BrowserWindow, WebBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 })
  // win.loadURL("https://bitbucket.org");

  const webContentView = new BaseView()
  webContentView.setBackgroundColor("#00ff00")
  webContentView.setBounds({x: 400, y: 200, width: 650, height: 650})
  win.addChildView(webContentView)
  webContentView.enableMouseEvents();
  webContentView.setMouseTrackingEnabled(true);

  webContentView.on('mouse-down', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webContentView - mouse-down; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webContentView.on('mouse-up', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webContentView - mouse-up; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webContentView.on('mouse-move', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webContentView - mouse-move; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webContentView.on('mouse-enter', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webContentView - mouse-enter; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webContentView.on('mouse-leave', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webContentView - mouse-leave; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  const webBrowserView = new WebBrowserView({
    webPreferences: {
      optimizeForScroll : true,
    }
  })
  webBrowserView.webContents.loadURL('https://electronjs.org')
  webBrowserView.setBounds({x: 0, y: 0, width: 600, height: 600})
  webContentView.addChildView(webBrowserView)

  webBrowserView.on('mouse-down', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webBrowserView - mouse-down; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webBrowserView.on('mouse-up', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webBrowserView - mouse-up; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webBrowserView.on('mouse-move', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webBrowserView - mouse-move; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webBrowserView.on('mouse-enter', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webBrowserView - mouse-enter; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });

  webBrowserView.on('mouse-leave', function (_event, timestamp, button, positionInView, positionInWindow) {
    console.log('webBrowserView - mouse-leave; button: ' + button + ', positionInView: (' + positionInView.x + ', ' + positionInView.y + '); positionInWindow: (' + positionInWindow.x + ', ' + positionInWindow.y + ')');
  });
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
