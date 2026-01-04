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
      preload: path.join(__dirname, "titlebar.js"),
      nodeIntegration: true,
      contextIsolation: false,
    },
  });
  win.loadFile("view/index.html");
}

let maximizeToggle = false;

ipcMain.on(`btnminimize`, () => {
  win.minimize();
});

ipcMain.on(`btnmaximize`, () => {
  if (maximizeToggle) {
    win.unmaximize(); // if button is unclicked
  } else {
    win.maximize();
  }

  maximizeToggle = !maximizeToggle; // flip the value, ig
});

ipcMain.on(`btnclose`, () => {
  app.quit();
});

app.whenReady().then(UIWindow);
