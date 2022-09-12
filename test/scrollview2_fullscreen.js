// WebBrowserViews in scroll, use setContentBaseView for BrowserWindow

const path = require("path");
const { app, BaseView, BaseWindow, BrowserWindow, ScrollView, WebBrowserView, screen } = require("electron");

const APP_WIDTH = 600;
const GAP = 30;

const APPS = [
  "https://bitbucket.org",
  "https://github.com",
  "https://figma.com/",
  "https://www.youtube.com/watch?v=AEh05NH9wiQ",
  "https://thenextweb.com/",
  "https://engadget.com/",
  "https://theverge.com/",
  "https://mashable.com",
  "https://www.businessinsider.com",
  "https://wired.com",
  "https://macrumors.com",
  "https://gizmodo.com",
  "https://maps.google.com/",
  "https://sheets.google.com/"
];

global.win = null;

function createWindow() {
  // Create window.
  win = new BrowserWindow({ autoHideMenuBar: true, width: 1400, height: 1000 });

  // The content view.
  const contentView = new BaseView();
  //contentView.setStyle({ flexDirection: "row", backgroundColor: "#1F2937" });
  contentView.setBackgroundColor("#1F2937");
  contentView.setBounds({ x: 0, y: 0, width: 1400, height: 1000 });

  win.setContentBaseView(contentView);

  // Scroll
  const scroll = new ScrollView({ smoothScroll: true, blockScrollViewWhenFocus: true });
  //scroll.setStyle({
    //flex: 1,
  //});
  scroll.setBounds({ x: 0, y: 0, width: 1400, height: 1000 });
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
  scroll.setContentSize({ width: APPS.length * (APP_WIDTH + GAP), height: 1000 });
  scrollContent.setBackgroundColor("#FF0000");

  win.on('resized', () => {
    const b = win.getBounds();
    console.log("resized - new bounds: {" + b.x + ", " + b.y + ", " + b.width + ", " + b.height + "}");
    contentView.setBounds({ ...b, x: 0, y: 0 });
    scroll.setBounds({ ...b, x: 0, y: 0 });

    const width = Math.max(APPS.length * (APP_WIDTH + GAP), b.width);
    scroll.setContentSize({ height: b.height, width });
    const pos = scroll.getScrollPosition();
    const max_pos = scroll.getMaximumScrollPosition();
    console.log("resized - scroll position: (" + pos.x + ", " + pos.y + "); maximum position: (" + max_pos.x + ", " + max_pos.y + ")");
    const content_size = scroll.getContentSize();
    console.log("resized - content size: (" + content_size.width + ", " + content_size.height + ")");
  });

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
    const webBrowserView = new WebBrowserView({
      webPreferences: {
        optimizeForScroll: true,
      }
    });
    webBrowserView.webContents.loadURL(url);
    webBrowserView.setBackgroundColor("#ffffff");
    webBrowserView.setBounds({ x: 0, y: 0, width: 600, height: 900 });
    const webContentView = new BaseView();
    webContentView.setBounds({ x: i * (APP_WIDTH + GAP) + GAP, y: 30, width: 600, height: 900 });
    webContentView.addChildView(webBrowserView);
    scrollContent.addChildView(webContentView);

    webBrowserView.webContents.on('enter-html-full-screen', function () {
      const { bounds: screenBounds } = screen.getDisplayMatching(win.getBounds());
      console.log("enter-html-full-screen - screenBounds: {" + screenBounds.x + ", " + screenBounds.y + ", " + screenBounds.width + ", " + screenBounds.height + "}");
      webContentView.removeChildView(webBrowserView);
      win.addChildView(webBrowserView);
      webBrowserView.setBounds({ ...screenBounds, x: 0, y: 0 });
      webBrowserView.webContents.focus();
      const pos = scroll.getScrollPosition();
      const max_pos = scroll.getMaximumScrollPosition();
      console.log("scroll position after entering full screen: (" + pos.x + ", " + pos.y + "); maximum position: (" + max_pos.x + ", " + max_pos.y + ")");
      const content_size = scroll.getContentSize();
      console.log("content size after entering full screen: (" + content_size.width + ", " + content_size.height + ")");

      const leave = () => {
        webBrowserView.webContents.executeJavaScript('document.webkitExitFullscreen();');
        win.removeChildView(webBrowserView);
        webContentView.addChildView(webBrowserView);
        webBrowserView.setBounds({ x: 0, y: 0, width: 600, height: 900 });
        webBrowserView.webContents.focus();
        const pos2 = scroll.getScrollPosition();
        const max_pos2 = scroll.getMaximumScrollPosition();
        console.log("scroll position after leaving full screen: (" + pos2.x + ", " + pos2.y + "); maximum position: (" + max_pos2.x + ", " + max_pos2.y + ")");
        const content_size2 = scroll.getContentSize();
        console.log("content size after leaving full screen: (" + content_size2.width + ", " + content_size2.height + ")");
      }
      win.once('leave-full-screen', leave);
      win.once('leave-html-full-screen', leave);
    });

    if (url.includes('youtube.com/watch')) {
      setTimeout(async () => {
        try {
          scroll.setScrollPosition({ y: 0, x: i * (APP_WIDTH + GAP) + GAP }).then(() => {
            const pos = scroll.getScrollPosition();
            const max_pos = scroll.getMaximumScrollPosition();
            console.log("scroll position before entering full screen: (" + pos.x + ", " + pos.y + "); maximum position: (" + max_pos.x + ", " + max_pos.y + ")");
            setTimeout(() => {
              win.setFullScreen(true);
              webBrowserView.webContents.emit('enter-html-full-screen');

              setTimeout(() => {
                win.setFullScreen(false);
                win.emit('leave-html-full-screen');
              }, 2000);
            }, 1000);
          });
        } catch (error) {
          console.error(error);
        }
      }, 5000);
    }  };

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
