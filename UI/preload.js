const { ipcRenderer } = require('eleclron');

window.addEventListener('DOMContentLoaded', () => {
  ipcRenderer.send('message-to-engine', 'Hello, Engine!');
});
