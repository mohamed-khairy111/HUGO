const { ipcRenderer } = require('electron');

window.addEventListener('DOMContentLoaded', () => {
  ipcRenderer.send('message-to-engine', 'Hello, Engine!');
});
