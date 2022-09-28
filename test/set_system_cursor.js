const { app, BrowserWindow, WebContents } = require('electron')

var cursorIsSet = false;

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 });
  win.loadURL("https://github.com");

  win.webContents.on('cursor-changed', function (_event, type, image, scale, size, hotspot) {
    if (type == 'custom')
      console.log('cursor changed on custom! scale: ' + scale + ', size: (' + size.width + ', ' + size.height + '), hotspot: (' + hotspot.x + ', ' + hotspot.y + ')');
    else
      console.log('cursor changed! ' + type);
  });

  setTimeout(async () => {
    app.setSystemCursor(__dirname + '/cursor.png');
    cursorIsSet = true;
  }, 3000);

  setTimeout(async () => {
    if (cursorIsSet)
      app.restoreSystemCursor();
    cursorIsSet = false;
  }, 15000);
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});

app.on('will-quit', function () {
  if (cursorIsSet)
    app.restoreSystemCursor();
});
