const { ipcRenderer } = require('electron')

function start() {
    console.log("start")
    let args = "https://google.com"
    ipcRenderer.send('start::screenshot',args)
}
