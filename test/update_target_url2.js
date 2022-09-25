var path = require('path');
const { app, BaseWindow, BrowserWindow, WebBrowserView } = require('electron');

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 });

  win.webContents.loadFile(path.join(__dirname, 'update_target_url.html'));

  win.webContents.on('update-target-url', (_event, url, location) => {
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
