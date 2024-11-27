const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const engineBridge = require('../shared/engine-bridge');

let mainWindow;

app.on('ready', () => {
  app.commandLine.appendSwitch('disable-features', 'AutofillEnable');
  app.commandLine.appendSwitch(
    'disable-features',
    'AutofillServerCommunication'
  );

  engineBridge.startEngine();

  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
    },
  });
  mainWindow.loadFile('UI/index.html');

  ipcMain.on('message-to-engine', (event, message) => {
    console.log('Message from UI:', message);
    engineBridge.sendMessage(message);
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    engineBridge.stopEngine();
    app.quit();
  }
});
