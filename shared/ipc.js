const { ipcMain } = require('electron');

ipcMain.on('message-to-engine', (event, message) => {
  console.log('Message from UI: ', message);
});
