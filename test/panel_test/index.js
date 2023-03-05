var { app, BrowserWindow, globalShortcut } = require('electron')
var path = require('path')

var panelWindow = null
var mainWindow = null

app.on('ready', function () {

  mainWindow = new BrowserWindow({
    width: 1000,
    minWidth: 500,
    minHeight: 500,
    fullscreenable: false,
    paintWhenInitiallyHidden: true,
    show: false,
    frame: false,
    transparent: false,
    titleBarStyle: 'default',
    minimizable: true,
    maximizable: true,
    closable: true,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false,
      backgroundThrottling: false,
    },
  })

  mainWindow.loadURL('file://' + __dirname + '/index.html')
  mainWindow.on('ready-to-show', function () {
    mainWindow.showInactive()
  })

  panelWindow = new BrowserWindow({
    type: "panel",
    width: 100,
    minWidth: 100,
    minHeight: 100,
    paintWhenInitiallyHidden: true,
    show: false,
    frame: false,
    transparent: true,
    minimizable: false,
    maximizable: false,
    closable: false,
    alwaysOnTop: true,
    fullscreenable: true,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      enableRemoteModule: false,
      backgroundThrottling: false,
    },
  })
  panelWindow.setVisibleOnAllWorkspaces(true, { visibleOnFullScreen: true })
  panelWindow.setSize(200, 200)

  panelWindow.loadURL('file://' + __dirname + '/index.html')
  panelWindow.on('ready-to-show', function () {
    panelWindow.showInactive()
  })

  function showPanel () {
    panelWindow.showInactive()
    panelWindow.makeKeyWindow()

  }

  function hidePanel () {
    panelWindow.hide()
  }

  globalShortcut.unregisterAll()
  const shortcutRegistered = globalShortcut.register('cmd+shift+e', function () {
    if (panelWindow.isVisible()) {
      hidePanel()
    } else {
      showPanel()
    }
  })
  if (!shortcutRegistered) {
    console.warn('Global shortcut registration failed (cli)')
  }

  setTimeout(() => {
    showPanel()
  }, 1000)

  let closable = false
  app.on('before-quit', (e) => {
    if (!closable) {
      closable = true
      setTimeout(() => {
        mainWindow.setClosable(true)
        panelWindow.setClosable(true)
        app.quit()
      })
      e.preventDefault()
    }
  })
})