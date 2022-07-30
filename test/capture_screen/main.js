// Modules to control application life and create native browser window
const {app, BrowserWindow} = require('electron')
const { desktopCapturer } = require('electron')
const { systemPreferences } = require('electron')
const path = require('path')
const permissions = require('./permissions')

async function requestPermissions() {
  if (process.platform === 'darwin') {
    //const microphone_status = await systemPreferences.askForMediaAccess('microphone');
    //console.log('microphone permission granted: ' + microphone_status)
    //const camera_status = await systemPreferences.askForMediaAccess(`camera`);
    //console.log('camera permission granted: ' + camera_status)
    //const screen_status = await systemPreferences.askForMediaAccess(`screen`);
    //console.log('screen sharing permission granted: ' + screen_status)
    permissions.askForScreenCaptureAccess()
    console.log('systemPreferences.getMediaAccessStatus for microphone: ' + systemPreferences.getMediaAccessStatus('microphone'))
    console.log('systemPreferences.getMediaAccessStatus for camera: ' + systemPreferences.getMediaAccessStatus('camera'))
    console.log('systemPreferences.getMediaAccessStatus for screen: ' + systemPreferences.getMediaAccessStatus('screen'))
  }
}

function createWindow () {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  console.log('systemPreferences.getMediaAccessStatus for microphone: ' + systemPreferences.getMediaAccessStatus('microphone'))
  console.log('systemPreferences.getMediaAccessStatus for camera: ' + systemPreferences.getMediaAccessStatus('camera'))
  console.log('systemPreferences.getMediaAccessStatus for screen: ' + systemPreferences.getMediaAccessStatus('screen'))
  requestPermissions()

  desktopCapturer.getSources({ types: ['window', 'screen'] }).then(async sources => {
    for (const source of sources) {
      console.log("source - id: " + source.id + ", name: " + source.name + ", display_id: " + source.display_id);
    }
    for (const source of sources) {
      if (source.name.startsWith('Entire screen')) {
        mainWindow.webContents.send('SET_SOURCE', source.id)
        return
      }
    }
  })

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
