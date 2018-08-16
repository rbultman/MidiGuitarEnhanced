var midi = require('midi');

// Set up a new input.
var input = new midi.input();

// Count the available input ports.
var portCount = input.getPortCount();

console.log("Found this many ports: " + portCount);

function noteOffHandler(data) {
  if (data.length == 3) {
    var messageType = data[0] & 0xF0;
    var channel = (data[0] & 0x0F)+1;
    console.log("Channel: " + channel + ", note " + data[1] + ", Off, Velocity=" + data[2]);
  }
}

function noteOnHandler(data) {
  if (data.length == 3) {
    var messageType = data[0] & 0xF0;
    var channel = (data[0] & 0x0F)+1;
    console.log("Channel: " + channel + ", note " + data[1] + ", On, Velocity=" + data[2]);
  }
}

var statusMessageType = {
  0x80: {text: "Note off", handler: noteOffHandler},
  0x90: {text: "Note on", handler: noteOnHandler},
  0xA0: {text: "Polyphonic key pressure", handler: null},
  0xB0: {text: "Control change", handler: null},
  0xC0: {text: "Program change", handler: null},
  0xD0: {text: "Channel pressure", handler: null},
  0xE0: {text: "Pitch bend change", handler: null},
};

function decode(data) {
  var messageType = data[0] & 0xF0;
  var channel = (data[0] & 0x0F)+1;
  if (statusMessageType[messageType]) {
    if (statusMessageType[messageType].handler != null) {
      statusMessageType[messageType].handler(data);
    }
  }
}

input.on('message', function(deltaTime, message) {
  // The message is an array of numbers corresponding to the MIDI bytes:
  //   [status, data1, data2]
  // https://www.cs.cf.ac.uk/Dave/Multimedia/node158.html has some helpful
  // information interpreting the messages.
  //console.log('m:' + message + ' d:' + deltaTime);
  decode(message);
});

if (portCount > 0) {
  console.log("Names: ");
  for(var i=0; i<portCount; i++) console.log("Port " + i + " name: " + input.getPortName(i));
  input.openPort(0);
}

// Sysex, timing, and active sensing messages are ignored
// by default. To enable these message types, pass false for
// the appropriate type in the function below.
// Order: (Sysex, Timing, Active Sensing)
// For example if you want to receive only MIDI Clock beats
// you should use
// input.ignoreTypes(true, false, true)
input.ignoreTypes(false, false, false);

