// WebBrowserViews in scroll, use setContentBaseView for BaseWindow
// 1. for each webBrowserView: captures and shows thumbnail
// 2. zoom out
// 3. for each webBrowserView hide thumbnail

const fs = require('fs')
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

const MIDDLE = APPS.length / 2;

global.win = null;
global.webBrowserViews = Array(APPS.length);
global.webContentViews = Array(APPS.length);

function addMoreWebContentViews(view, view2) {
  console.log("@@addMoreWebContentViews");
  for (var i = MIDDLE; i < MIDDLE+1; i++)
    addWebview(view2, APPS[i], i);
  setTimeout(showWebBrowserViews, 10000, view, view2)
}

function showWebBrowserViews(view, view2) {
  for (var i = 0; i < MIDDLE; i++)
    webBrowserViews[i].show();
  for (var i = 0; i < MIDDLE; i++) {
    const bounds = webBrowserViews[i].getBounds()
    console.log("bounds for " + i + " after showing: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
    const   viewBounds = webBrowserViews[i].getViewBounds()
    console.log("viewBounds for " + i + " after showing: (" + viewBounds.x + ", " + viewBounds.y + ", " + viewBounds.width + ", " + viewBounds.height + ")")
    const scaleX = webBrowserViews[i].getScaleX()
    const scaleY = webBrowserViews[i].getScaleY()
    console.log("scale for " + i + " after showing: (" + scaleX + ", " + scaleY + ")")
  }
  //setTimeout(addMoreWebContentViews, 10000, view, view2)
}

function finishZoomIn(view, view2) {
  let bounds = view.getBounds()
  console.log("bounds after zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  let viewBounds = view.getViewBounds()
  console.log("viewBounds after zooming in: (" + viewBounds.x + ", " + viewBounds.y + ", " + viewBounds.width + ", " + viewBounds.height + ")")
  let scaleX = view.getScaleX()
  let scaleY = view.getScaleY()
  console.log("scale after zooming in: (" + scaleX + ", " + scaleY + ")")
  for (var i = 0; i < MIDDLE; i++) {
    bounds = webBrowserViews[i].getBounds()
    console.log("bounds for " + i + " after zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
    viewBounds = webBrowserViews[i].getViewBounds()
    console.log("viewBounds for " + i + " after zooming in: (" + viewBounds.x + ", " + viewBounds.y + ", " + viewBounds.width + ", " + viewBounds.height + ")")
    scaleX = webBrowserViews[i].getScaleX()
    scaleY = webBrowserViews[i].getScaleY()
    console.log("scale for " + i + " after zooming in: (" + scaleX + ", " + scaleY + ")")
  }

  setTimeout(addMoreWebContentViews, 10000, view, view2)
}

function startZoomIn(view, view2) {
  let bounds = view.getBounds()
  console.log("bounds before zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
  let viewBounds = view.getViewBounds()
  console.log("viewBounds before zooming in: (" + viewBounds.x + ", " + viewBounds.y + ", " + viewBounds.width + ", " + viewBounds.height + ")")
  let scaleX = view.getScaleX()
  let scaleY = view.getScaleY()
  console.log("scale before zooming in: (" + scaleX + ", " + scaleY + ")")
  for (var i = 0; i < MIDDLE; i++) {
    bounds = webBrowserViews[i].getBounds()
    console.log("bounds for " + i + " before zooming in: (" + bounds.x + ", " + bounds.y + ", " + bounds.width + ", " + bounds.height + ")")
    viewBounds = webBrowserViews[i].getViewBounds()
    console.log("viewBounds for " + i + " before zooming in: (" + viewBounds.x + ", " + viewBounds.y + ", " + viewBounds.width + ", " + viewBounds.height + ")")
    scaleX = webBrowserViews[i].getScaleX()
    scaleY = webBrowserViews[i].getScaleY()
    console.log("scale for " + i + " before zooming in: (" + scaleX + ", " + scaleY + ")")
  }

  view.setScale({"scaleX": 0.2, "scaleY": 0.2, "adjustFrame": true, "animation": {"duration": 5, timingFunction: "easeIn"}, "anchorX": "left", "anchorY": "top"})
  setTimeout(finishZoomIn, 6000, view, view2)
}

  function addWebview(scrollContent, url, i) {
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
        "show": false,
      }
    });
    console.log("@creating " + webBrowserView.id + " webBrowserView");
    webBrowserViews[i] = webBrowserView;
    webBrowserView.webContents.loadURL(url);
    webBrowserView.setBackgroundColor("#ffffff");
    webBrowserView.setBounds({x: 0, y: 0, width: 600, height: 540});
    webBrowserView.setBlockResizing(true);
    const webContentView = new BaseView();
    webContentView.setBounds({x: i*(APP_WIDTH + GAP)+GAP, y: 30, width: 600, height: 540});
    webContentView.addChildView(webBrowserView);
    webContentViews[i] = webContentView;
    scrollContent.addChildView(webContentView);
  }

function createWindow () {
  // Create window.
  win = new BaseWindow({ autoHideMenuBar: true, width: 1400, height: 1000 });

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

  contentView.addChildView(scroll);

  // Scroll content
  const scrollContent = new BaseView();
  //scrollContent.setStyle({
    //flexDirection: "row",
    //flex: 1,
    //padding: 30,
    //flexShrink: 0,
  //});
  scrollContent.setBackgroundColor("#FF0000");
  scroll.setContentView(scrollContent);
  scroll.setContentSize({ width: APPS.length * (APP_WIDTH + GAP), height: 600 });

  const scrollContent2 = new BaseView();
  scrollContent2.setBounds({x: 0, y: 0, width: APPS.length * (APP_WIDTH + GAP), height: 600});
  scrollContent.addChildView(scrollContent2);

  for (var i = 0; i < MIDDLE; i++)
    addWebview(scrollContent2, APPS[i], i);

  for (var i = 0; i < MIDDLE; i++)
    captureScreenshotForApp(i);

  setTimeout(startZoomIn, 10000, scrollContent, scrollContent2)
}

function captureScreenshotForApp(index) {
  console.log('capturing ' + index + ' page...')
  let offscreenWindow = new BrowserWindow({
      width: 600,
      height: 540,
      show: false,
      webPreferences: {
          offscreen: true
      }
  });

  offscreenWindow.loadURL(APPS[index]);
  offscreenWindow.webContents.on('did-finish-load', async() => {
    let start_hrtime = process.hrtime();
    // Get screenshot (thumbnail)
    let nativeImage = await offscreenWindow.webContents.captureScreenshot().then(image => {
      let stop_hrtime = process.hrtime(start_hrtime);
      console.log(`Time Taken to screenshot for ${APPS[index]}: ${(stop_hrtime[0] * 1e9 + stop_hrtime[1])/1e9} seconds`);
      fs.writeFileSync('test' + index + '.png', image.toPNG(), (err) => {
        if (err) throw err;
      });
      console.log('It\'s saved!');
      webBrowserViews[index].hide(true, image);
      return image.toDataURL();
    });
  });
}

function captureScreenshots() {
    offscreenWindow = new BrowserWindow({
        width: 600,
        height: 540,
        show: false,
        webPreferences: {
            offscreen: true
        }
    })
  captureScreenshotForApp(0)
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
