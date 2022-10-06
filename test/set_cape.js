const { app, BrowserWindow, WebContents } = require('electron')

var capeIsApplied = false;

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 });
  win.loadURL("https://github.com");

  win.webContents.on('cursor-changed', function (_event, type, image, scale, size, hotspot) {
    if (type == 'custom')
      console.log('cursor changed on custom! scale: ' + scale + ', size: (' + size.width + ', ' + size.height + '), hotspot: (' + hotspot.x + ', ' + hotspot.y + ')');
    else
      console.log('cursor changed! ' + type);
  });

  if (process.platform === 'darwin') {
    setTimeout(async () => {
      app.setCape(__dirname + '/com.maxrudberg.svanslosbluehazard.cape');
      capeIsApplied = true;
    }, 3000);

    setTimeout(async () => {
      if (capeIsApplied)
        app.resetCape();
      capeIsApplied = false;
    }, 15000);
  }
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});

app.on('will-quit', function () {
  if (capeIsApplied)
    app.resetCape();
});
