const { app, BrowserWindow, WebBrowserView } = require('electron')

app.whenReady().then(() => {
  const win = new BrowserWindow({
    x: 600,
    y: 600,
  })

  win.loadFile('index.html')

  const view = new WebBrowserView()
  win.addChildView(view)

  view.setBounds({ x: 0, y: 0, width: 300, height: 300 })

  view.webContents.loadURL('https://www.google.com')

  view.webContents.openDevTools({ mode: 'detach' })

  setTimeout(() => {
    view.webContents.close()
  }, 3000)
})
