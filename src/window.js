/*
RIGHTFULLY MADE BY HUANG WIE LIM (RAYMONDOES) AND MARTIN (MONSIEUR DR1P)
*/

const { app, BrowserWindow, ipcMain } = require("electron");
const path = require("path");

let win;

function UIWindow() {
  win = new BrowserWindow({
    width: 1160,
    height: 640,
    autoHideMenuBar: true,
    frame: false,
    webPreferences: {
      preload: path.join(__dirname, "preload.js"),
    },
  });
  win.loadFile("view/index.html");
}

app.whenReady().then(UIWindow);
