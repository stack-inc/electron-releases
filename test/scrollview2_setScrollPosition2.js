// WebBrowserViews in scroll, use setContentBaseView for BrowserWindow

const path = require("path");
const { app, BaseView, BaseWindow, BrowserWindow, ScrollView, WebBrowserView } = require("electron");

const APP_WIDTH = 600;
const APP_HEIGHT = 540;
const GAP = 30;

const APPS = [
  "https://bitbucket.org",
  "https://github.com",
  "https://youtube.com",
  "https://figma.com/",
  "https://thenextweb.com/",
  "https://engadget.com/",
  "https://theverge.com/",
  "https://en.wikipedia.org/",
];

const APPS2 = [
  "https://mashable.com",
  "https://www.businessinsider.com",
  "https://wired.com",
  "https://macrumors.com",
  "https://gizmodo.com",
  "https://maps.google.com/",
  "https://sheets.google.com/",
  "https://simple.wikipedia.org/",
];

global.win = null;

global.pos_counter = 0;

function setScrollPosition(scroll) {
  max_pos = scroll.getMaximumScrollPosition();
  new_pos_x = Math.floor(Math.random() * max_pos.x+100); 
  new_pos_y = Math.floor(Math.random() * max_pos.y+100); 
  scroll.setScrollPosition({x: new_pos_x, y:new_pos_y}).then(() => {
    pos = scroll.getScrollPosition();
    console.log("New position: (" + new_pos_x + ", " + new_pos_y + "); position after scrolling: (" + pos.x + ", " + pos.y + "); maximum position: (" + max_pos.x + ", " + max_pos.y + ")");
    expected_pos_x = (new_pos_x > max_pos.x) ? max_pos.x : new_pos_x;
    expected_pos_y = (new_pos_y > max_pos.y) ? max_pos.y : new_pos_y;
    if (pos.x != expected_pos_x || pos.y != expected_pos_y)
      console.log("error! expected: (" + expected_pos_x + ", " + expected_pos_y + ")");
    pos_counter++;
    if (pos_counter < 1000)
      setTimeout(setScrollPosition, 50, scroll)
  });
}

function createWindow () {
  // Create window.
  win = new BrowserWindow({ autoHideMenuBar: true, width: 1400, height: 1000 });

  // The content view.
  const contentView = new BaseView();
  //contentView.setStyle({ flexDirection: "row", backgroundColor: "#1F2937" });
  contentView.setBackgroundColor("#1F2937");
  contentView.setBounds({x: 0, y: 0, width: 1378, height: 600});

  win.setContentBaseView(contentView);

  // Scroll
  const scroll = new ScrollView({ smoothScroll: true, blockScrollViewWhenFocus: true });
  //scroll.setStyle({
    //flex: 1,
  //});
  scroll.setBounds({x: 0, y: 0, width: 1377, height: 600});
  scroll.setHorizontalScrollBarMode("enabled");
  scroll.setVerticalScrollBarMode("disabled");
  scroll.setScrollWheelSwapped(true);

  contentView.addChildView(scroll);

  // Scroll content
  const scrollContent = new BaseView();
  //scrollContent.setStyle({
    //flexDirection: "row",
    //flex: 1,
    //padding: 30,
    //flexShrink: 0,
  //});
  scroll.setContentView(scrollContent);
  scroll.setContentSize({ width: APPS.length * (APP_WIDTH + GAP), height: 1200 });
  scrollContent.setBackgroundColor("#FF0000");

  // Webview
  const addWebview = function (scrollContent, url, column, row) {
    //chrome.setStyle({ flex: 1 });
    //const webContentView = gui.Container.create();
    //webContentView.setStyle({
      //width: APP_WIDTH,
      //height: "100%",
      //backgroundColor: "#ffffff",
      //marginRight: GAP,
    //});
    //webContentView.addChildView(chrome);
    const webBrowserView = new WebBrowserView({
      webPreferences: {
        optimizeForScroll : true,
      }
    });
    webBrowserView.webContents.loadURL(url);
    webBrowserView.setBackgroundColor("#ffffff");
    webBrowserView.setBounds({x: 0, y: 0, width: APP_WIDTH, height: APP_HEIGHT});
    const webContentView = new BaseView();
    webContentView.setBounds({x: column*(APP_WIDTH + GAP)+GAP, y: row*(APP_HEIGHT + GAP)+GAP, width: APP_WIDTH, height: APP_HEIGHT});
    webContentView.addChildView(webBrowserView);
    scrollContent.addChildView(webContentView);
  };

  var i = 0;
  APPS.forEach((app) => {
    addWebview(scrollContent, app, i++, 0);
  });
  i = 0;
  APPS.forEach((app) => {
    addWebview(scrollContent, app, i++, 1);
  });

  setTimeout(setScrollPosition, 6000, scroll)
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
