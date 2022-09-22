const { app, BrowserWindow, WebContents } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 });
  win.loadURL("https://github.com");

  cursor_was_set = false;
  win.webContents.on('cursor-changed', function (_event, type, image, scale, size, hotspot) {
    if (type == 'custom')
      console.log('cursor changed on custom! scale: ' + scale + ', size: (' + size.width + ', ' + size.height + '), hotspot: (' + hotspot.x + ', ' + hotspot.y + ')');
    else
      console.log('cursor changed! ' + type);
    if (type == 'default' || type == 'pointer') {
      win.webContents.setCursor(__dirname + '/cursor.png');
      cursor_was_set = true;
    }
  });

  setTimeout(async () => {
    if (!cursor_was_set)
      win.webContents.setCursor(__dirname + '/cursor.png');
  }, 3000);
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
