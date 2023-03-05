const { BrowserWindow } = require('electron')
const fs = require('fs')
const path = require('path')
// Offscreen BrowserWindow
let offscreenWindow
let nativeImage
// Exported readItem function
module.exports = (url, callback) => {

    // Create offscreen window
    offscreenWindow = new BrowserWindow({
        width: 500,
        height: 500,
        show: false,
        webPreferences: {
            offscreen: true
        }
    })

    // Load item url
    offscreenWindow.loadURL(url)
    console.log("readitem")

    // Wait for content to finish loading
    offscreenWindow.webContents.on('did-stop-loading', async() => {

        // Get page title
        let title = offscreenWindow.getTitle()
        console.log(title)

        let start_hrtime = process.hrtime();
        // Get screenshot (thumbnail)
        nativeImage = await offscreenWindow.webContents.captureScreenshot().then(image => {
            let stop_hrtime = process.hrtime(start_hrtime)
            console.log(`Time Taken to execute webContents.capturePage: ${(stop_hrtime[0] * 1e9 + stop_hrtime[1])/1e9} seconds`)
            fs.writeFileSync('test.png', image.toPNG(), (err) => {
                if (err) throw err
            })
            console.log('It\'s saved!')
            return image.toDataURL()
        })

        start_hrtime = process.hrtime();
        // Get screenshot (thumbnail)
        nativeImage = await offscreenWindow.webContents.captureScreenshot().then(image => {
            let stop_hrtime = process.hrtime(start_hrtime)
            console.log(`Time Taken to execute webContents.capturePage: ${(stop_hrtime[0] * 1e9 + stop_hrtime[1])/1e9} seconds`)
            fs.writeFileSync('test.png', image.toPNG(), (err) => {
                if (err) throw err
            })
            console.log('It\'s saved!')
            return image.toDataURL()
        })

        let obj = { title:title, 
            url: url,
            image: nativeImage }
        callback(obj) 
        // Clean up
        offscreenWindow.close()
        offscreenWindow = null
    })
}
