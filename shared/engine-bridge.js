const { spawn } = require('child_process'); // Import the 'spawn' function from the 'child_process' module.
const path = require('path'); // Import the 'path' module for working with file paths.

class EngineBridage {
  constructor() {
    this.engineProcess = null; // Initialize the 'engineProcess' property to null initially.
  }

  startEngine() {
    // Start the engine process using the 'spawn' function.
    this.engineProcess = spawn(path.join(__dirname, '../engine/engine.exe'));

    // Event listener for standard output from the engine process.
    this.engineProcess.stdout.on('data', (data) => {
      console.log(`engine output: ${data.toString()}`); // Log the output from the engine to the console.
    });

    // Event listener for when the engine process exits.
    this.engineProcess.on('exit', (code) => {
      console.log(`Engine process exited with code ${code}`);
    });
  }

  sendMessage(message) {
    try {
      // Send a message to the engine process through its standard input.
      this.engineProcess.stdin.write(message, 'utf8');
    } catch (error) {
      // Handle potential errors during message sending.
      console.error('Failed to send message to engine:', error);
    }
  }

  stopEngine() {
    if (this.engineProcess) {
      // Send the 'exit' command to the engine process to gracefully stop it.
      this.engineProcess.stdin.write('exit\n');
      this.engineProcess = null; // Set 'engineProcess' to null to indicate that the engine is no longer running.
    }
  }
}

// Export an instance of the EngineBridage class.
module.exports = new EngineBridage();
