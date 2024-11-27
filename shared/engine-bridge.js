const { spawn } = require('child_process');
const path = require('path');

class EngineBridage {
  constructor() {
    this.engineProcess = null;
  }

  startEngine() {
    this.engineProcess = spawn(path.join(__dirname, '../engine/engine.exe'));

    this.engineProcess.stdout.on('data', (data) => {
      console.log(`engine output: ${data.toString()}`);
    });

    this.engineProcess.on('exit', (code) => {
      console.log(`Engine process exited with code ${code}`);
    });
  }

  sendMessage(message) {
    try {
      this.engineProcess.stdin.write(message, 'utf8');
    } catch (error) {
      console.error('Failed to send message to engine:', error);
    }
  }

  stopEngine() {
    if (this.engineProcess) {
      this.engineProcess.stdin.write('exit\n');
      this.engineProcess = null;
    }
  }
}

module.exports = new EngineBridage();
