const { app, BaseWindow, BrowserView, WebContents } = require('electron')

app.whenReady().then(() => {
  const win = new BaseWindow({ width: 1400, height: 1200 });
  const view = new BrowserView();
  view.webContents.loadURL("https://github.com");
  view.setBounds({ x: 0, y: 0, width: 300, height: 300 });
  win.addBrowserView(view);

  view.webContents.on('cursor-changed', function (_event, type, image, scale, size, hotspot) {
    if (type == 'custom')
      console.log('cursor changed on custom! scale: ' + scale + ', size: (' + size.width + ', ' + size.height + '), hotspot: (' + hotspot.x + ', ' + hotspot.y + ')');
    else
      console.log('cursor changed! ' + type);
  });

  setTimeout(async () => {
    win.setCursor(__dirname + '/cursor.png');
  }, 3000);
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
