// BrowserViews in scroll, use setContentBaseView for BaseWindow

const path = require("path");
const { app, BrowserView, BaseWindow, ContainerView, ScrollView, WrapperBrowserView } = require("electron");

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

const anchors = [
  {"posX": "left", "posY": "top"},
  {"posX": "left", "posY": "center"},
  {"posX": "left", "posY": "bottom"},
];
let anchorIndex = 0

function finishZoomOut(view) {
  bounds = view.getBounds()
  console.log("bounds after zooming out: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  const scaleX = view.getScaleX()
  const scaleY = view.getScaleY()
  console.log("scale after zooming out: (" + scaleX + ", " + scaleY + ")")

  if (anchorIndex < anchors.length) {
    view.setScale({"scaleX": 0.5, "scaleY": 0.5, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
    setTimeout(finishZoomIn, 6000, view)
  }
}

function finishZoomIn(view) {
  bounds = view.getBounds()
  console.log("bounds after zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  const scaleX = view.getScaleX()
  const scaleY = view.getScaleY()
  console.log("scale after zooming in: (" + scaleX + ", " + scaleY + ")")

  view.setScale({"scaleX": 1.0, "scaleY": 1.0, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
  anchorIndex++;
  setTimeout(finishZoomOut, 6000, view)
}

function startZoomIn(view) {
  bounds = view.getBounds()
  console.log("bounds before zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  const scaleX = view.getScaleX()
  const scaleY = view.getScaleY()
  console.log("scale before zooming in: (" + scaleX + ", " + scaleY + ")")

  view.setScale({"scaleX": 0.5, "scaleY": 0.5, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": anchors[anchorIndex]["posX"], "anchorY": anchors[anchorIndex]["posY"]})
  setTimeout(finishZoomIn, 6000, view)
}

function createWindow () {
  // Create window.
  win = new BaseWindow({ autoHideMenuBar: true, width: 1400, height: 1000 });

  // The content view.
  const contentView = new ContainerView();
  //contentView.setStyle({ flexDirection: "row", backgroundColor: "#1F2937" });
  contentView.setBackgroundColor("#1F2937");
  contentView.setBounds({x: 0, y: 0, width: 1378, height: 600});

  win.setContentBaseView(contentView);

  // Scroll
  const scroll = new ScrollView();
  //scroll.setStyle({
    //flex: 1,
  //});
  scroll.setBounds({x: 0, y: 0, width: 1377, height: 600});
  scroll.setHorizontalScrollBarMode("enabled");
  scroll.setVerticalScrollBarMode("disabled");

  contentView.addChildView(scroll);

  // Scroll content
  const scrollContent = new ContainerView();
  //scrollContent.setStyle({
    //flexDirection: "row",
    //flex: 1,
    //padding: 30,
    //flexShrink: 0,
  //});
  scrollContent.setBackgroundColor("#FF0000");
  scrollContent.setBounds({x: 0, y: 0, width: APPS.length * (APP_WIDTH + GAP), height: 600});
  scroll.setContentView(scrollContent);

  // Webview
  const addWebview = function (scrollContent, url, i) {
    //chrome.setStyle({ flex: 1 });
    //const webContentView = gui.Container.create();
    //webContentView.setStyle({
      //width: APP_WIDTH,
      //height: "100%",
      //backgroundColor: "#ffffff",
      //marginRight: GAP,
    //});
    //webContentView.addChildView(chrome);
    const browserView = new BrowserView();
    browserView.webContents.loadURL(url);
    browserView.setBackgroundColor("#ffffff");
    const wrapperBrowserView = new WrapperBrowserView({ 'browserView': browserView });
    wrapperBrowserView.setBounds({x: 0, y: 0, width: 600, height: 540});
    const webContentView = new ContainerView();
    webContentView.setBounds({x: i*(APP_WIDTH + GAP)+GAP, y: 30, width: 600, height: 540});
    webContentView.addChildView(wrapperBrowserView);
    scrollContent.addChildView(webContentView);

  };

  var i = 0;
  APPS.forEach((app) => {
    addWebview(scrollContent, app, i++);
  });

  setTimeout(startZoomIn, 6000, scrollContent)
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
