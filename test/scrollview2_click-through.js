// WebBrowserViews in scroll, use setContentBaseView for BrowserWindow
const path = require("path");
const { app, BaseView, BaseWindow, BrowserWindow, ScrollView, WebBrowserView } = require("electron");

const APP_WIDTH = 600;
const GAP = 30;

const APPS = [
  "https://bitbucket.org",
  "https://github.com",
  "https://youtube.com",
  "https://figma.com/",
  "https://thenextweb.com/",
  "https://engadget.com/",
  "https://theverge.com/",
  "https://mashable.com",
  "https://www.businessinsider.com",
  "https://wired.com",
  "https://macrumors.com",
  "https://gizmodo.com",
  "https://maps.google.com/",
  "https://sheets.google.com/",
];

global.win = null;

function createWindow () {
  // Create window.
  win = new BrowserWindow({ autoHideMenuBar: true, width: 1400, height: 1200, optimizeForScroll: true });

  // The content view.
  const contentView = new BaseView();
  contentView.setBackgroundColor("#1d697c");
  contentView.setBounds({x: 0, y: 0, width: 1378, height: 800});

  win.setContentBaseView(contentView);

  // Scroll
  const scroll = new ScrollView({ smoothScroll: true, blockScrollViewWhenFocus: true });
  //scroll.setStyle({
    //flex: 1,
  //});
  scroll.setBounds({x: 0, y: 0, width: 1377, height: 800});
  scroll.setHorizontalScrollBarMode("enabled");
  scroll.setVerticalScrollBarMode("disabled");
  scroll.setScrollWheelSwapped(true);

  contentView.addChildView(scroll);

  // Scroll content
  const scrollContent = new BaseView();
  scroll.setContentView(scrollContent);
  scroll.setContentSize({width: APPS.length * (APP_WIDTH + GAP), height: 800});
  scrollContent.setBackgroundColor("#3300aa");

  // Click Through
  scrollContent.clickThrough = true;
  scroll.setScrollEventsEnabled(true);
  scroll.on('did-scroll', () => console.log('did-scroll'));

  // Webview
  const addWebview = function (scrollContent, url, i) {
    const webBrowserView = new WebBrowserView({
      webPreferences: {
        optimizeForScroll : true,
      }
    });
      webBrowserView.webContents.loadURL(url);
    webBrowserView.setBounds({x: 0, y: 50, width: 600, height: 540});
    const webContentView = new BaseView();
    webContentView.setBounds({x: i*(APP_WIDTH + GAP)+GAP, y: 30, width: 600, height: 740});
    webContentView.addChildView(webBrowserView);
    scrollContent.addChildView(webContentView);
  };

  var i = 0;
  APPS.forEach((app) => {
    addWebview(scrollContent, app, i++);
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow();

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BaseWindow.getAllWindows().length === 0)
      createWindow();
  });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin')
    app.quit();
});
