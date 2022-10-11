// Test of the rearranging algorithm  for the case where zIndex of the child is set before being added to its parent.
const { app, BaseView, BaseWindow, BrowserWindow } = require('electron');

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 1400, height: 1200 });
  // win.loadURL("https://bitbucket.org");

  const contents = new BaseView();
  contents.setBounds({x: 0, y: 0, width: 1300, height: 1100});
  win.addChildView(contents);

  for (let i = 0; i < 20; i++) {
    const webContentView = new BaseView();
    webContentView.zIndex = Math.floor(Math.random() * 10) + 1;
    webContentView.setBounds({x: 50, y: (i * 25) + 50, width: 250, height: 20});
    contents.addChildView(webContentView);
  }

  console.log('contents.getViews:');
  let views = contents.getViews();
  for (let i = 0; i < views.length; i++)
    console.log('id: ' + views[i].id + ', nativeId: ' + views[i].nativeId + ', zIndex: ' + views[i].zIndex);

  let rearrangedViews = contents.getRearrangedViews();
  let nativelyRearrangedViews = contents.getNativelyRearrangedViews();
  if (rearrangedViews.length != nativelyRearrangedViews.length)
    console.log('Error! rearrangedViews.length != nativelyRearrangedViews.length');
  console.log('contents.getRearrangedViews:');
  for (let i = 0; i < rearrangedViews.length; i++) {
    console.log('id: ' + rearrangedViews[i].id + ', nativeId: ' + rearrangedViews[i].nativeId + ', zIndex: ' + rearrangedViews[i].zIndex);
    if (rearrangedViews[i].id != nativelyRearrangedViews[i].id || rearrangedViews[i].nativeId != nativelyRearrangedViews[i].nativeId || rearrangedViews[i].zIndex != nativelyRearrangedViews[i].zIndex)
      console.log('Error! nativelyRearrangedViews - id: ' + nativelyRearrangedViews[i].id + ', nativeId: ' + nativelyRearrangedViews[i].nativeId + ', zIndex: ' + nativelyRearrangedViews[i].zIndex);
  }

  contents.rearrangeChildViews();

  let rearrangedViews2 = contents.getRearrangedViews();
  let nativelyRearrangedViews2 = contents.getNativelyRearrangedViews();
  if (rearrangedViews2.length != nativelyRearrangedViews2.length)
    console.log('Error! rearrangedViews2.length != nativelyRearrangedViews2.length');
  for (let i = 0; i < rearrangedViews.length; i++) {
    if (rearrangedViews[i].id != rearrangedViews2[i].id || rearrangedViews[i].nativeId != rearrangedViews2[i].nativeId || rearrangedViews[i].zIndex != rearrangedViews2[i].zIndex)
    console.log('id: ' + rearrangedViews2[i].id + ', nativeId: ' + rearrangedViews2[i].nativeId + ', zIndex: ' + rearrangedViews2[i].zIndex);
    if (rearrangedViews2[i].id != nativelyRearrangedViews2[i].id || rearrangedViews2[i].nativeId != nativelyRearrangedViews2[i].nativeId || rearrangedViews2[i].zIndex != nativelyRearrangedViews2[i].zIndex)
      console.log('Error! nativelyRearrangedViews - id: ' + nativelyRearrangedViews2[i].id + ', nativeId: ' + nativelyRearrangedViews2[i].nativeId + ', zIndex: ' + nativelyRearrangedViews2[i].zIndex);
  }
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
