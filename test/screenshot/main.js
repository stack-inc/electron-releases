const {app, BrowserWindow, ipcMain } = require('electron')
const path = require("path")
const fs = require("fs")
const screenshot = require("./screenshot")
let  window 

function createWindow () {
    window = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: true
    }
  })
  window.loadFile("./index.html")
  //window.webContents.openDevTools()
screenshot('https://bitbucket.org/product/', (reply) => {
console.log("It's completed", reply)
})
}

app.whenReady().then(createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

ipcMain.on("start::screenshot", (event,arg) => {
  console.log("Starting")
  screenshot(arg, (reply) => {console.log("It's completed", reply)})
})
