var path = require('path');
const { app, BaseWindow, WebBrowserView } = require('electron');

app.whenReady().then(() => {
  const win = new BaseWindow({ width: 1400, height: 1200 });

  const webBrowserView = new WebBrowserView();
  webBrowserView.webContents.loadURL(path.join(__dirname, 'update_target_url.html'));
  webBrowserView.setBounds({x: 200, y: 200, width: 500, height: 500});
  win.addChildView(webBrowserView);

  webBrowserView.webContents.on('update-target-url', (_event, url, location) => {
    console.log('update-target - url: ' + url + '; location: (' + location.x + ', ' + location.y + ')');
  });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
