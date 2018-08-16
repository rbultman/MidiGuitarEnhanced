# MidiGuitarEnhanced
An enhancement to the RockBand3 Mustang Pro guitar

This hack was originally done by [gillspice](http://github.com/gillspice).  He took a Rockband 3 Mustang Pro guitar and hacked it to play left handed.  He also added cap touch to the strings so that notes can be muted by touching the strings.

This hack recreates the original hack, plus adds a switch to select left/right-handed play on the fly, as well as a second switch to force all of the notes to be played through a single MIDI channel (instead of 6 channels) for use with devices which expect all MIDI data to be on a single channel.

### How it works
An Arduino Uno sits between the MIDI jack in the guitar and the original controller board.  The Arduino either passes through MIDI notes for right-handed play or remaps the notes to the mirror-image channel and note for left-handed play.  For single-channel mode, all notes are simply sent on channel 1.

Note that an optocoupler is used to interface between the original controller board and the Arduino. See [The MIDI electrical spec](https://www.midi.org/specifications/item/midi-din-electrical-specification) for details.

The Aruino code makes use of the [Ardino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library/).

### I/O Map
I/O Pin | Function
--------|--------
D0 | TX from the original controller
D1 | TX to the MIDI jack
D2 | String 1 (E2) mute
D3 | String 2 (A2) mute
D4 | String 3 (D3) mute
D5 | String 4 (G3) mute
D6 | String 5 (B3) mute
D7 | String 6 (E4) mute
A5 | Left (LOW)/Right (HIGH) handed play
A4 | Single (LOW)/Multi (HIGH) channel output

### MIDI Echo Traffic Sniffer
A small application was made using node.js and the [node MIDI library](https://github.com/justinlatimer/node-midi) that prints the contents of the MIDI traffic if a USB MIDI adapter is connected between a computer and the MIDI jack on the guitar.  This was useful for development and debuging of the Arduino code.  It is in the midi-echo directory.  To use the sniffer, you will need node.js installed and a USB MIDI adapter that is supported by your operating system.  This little program was created on a Mac.  YMMV.

To "install" it, open a terminal in the midi-echo directory and type:
```
npm install
```

To use it, open a terminal in the midi-echo directory and type:
```
node index.js
```

To exit the application, type `<CTRL>C`.

### Notes

1. It should be possible to create an in-line left/right box without the mute capability and without altering the controller.
2. If you can't program the Arduino after connecting hardware to D0 and D1, disconnect the circuits from D0 and D1.

