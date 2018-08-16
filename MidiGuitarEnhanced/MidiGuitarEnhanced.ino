#include <MIDI.h>

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

#define STRING_COUNT 6
// cap touch input channels, strings 1-6
int muteInput[STRING_COUNT] = {2, 3, 4, 5, 6, 7};

// store mute state of each string
bool stringIsMuted[STRING_COUNT];

// switch inputs
#define LEFT_HANDED_SWITCH A5
#define ONE_CHANNEL_SWITCH A4
bool leftHanded = false;
bool oneChannel = false;

class Note {
  public:
  Note(int note=-1) {this->note = note;}
  void setNote(int note) {this->note = note;}
  int note;
  int velocity;
};

Note lastNote[STRING_COUNT];

byte swapChannel(byte channel) {
  const byte newChannel[] = {0, 6, 5, 4, 3, 2, 1};
  return newChannel[channel];
}

byte swapNote(byte channel, byte note) {
  const int offset[] = {0, -24, -14, -5, 5, 14, 24};
  int newNote = note + offset[channel];
  if (newNote < 0) {
    newNote = 0;
  } else if (newNote > 127) {
    newNote = 127;
  }
  return (byte)newNote;
}

void sendNoteOn(byte note, byte velocity, byte channel) {
  if (oneChannel) {
    MIDI.sendNoteOn(note, velocity, 1);
  } else {
    MIDI.sendNoteOn(note, velocity, channel);
  }
}

void sendNoteOff(byte note, byte velocity, byte channel) {
  if (oneChannel) {
    MIDI.sendNoteOff(note, velocity, 1);
  } else {
    MIDI.sendNoteOff(note, velocity, channel);
  }
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  if (leftHanded) {
    note = swapNote(channel, note);
    channel = swapChannel(channel);
  }
  if (!stringIsMuted[channel-1]) {
    sendNoteOn(note, velocity, channel);
    lastNote[channel-1].setNote(note);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (leftHanded) {
    note = swapNote(channel, note);
    channel = swapChannel(channel);
  }
  
  sendNoteOff(note, velocity, channel);
}

uint8_t readStrings(void) {
  uint8_t strings = 0;
  uint8_t string = 1;

  for(int i=0; i<STRING_COUNT; i++, string=string<<1) {
    if (digitalRead(muteInput[i]) == HIGH) {
      strings |= string;
    }
  }

  return strings;
}

void muteStrings(uint8_t oldStrings, uint8_t newStrings) {
  uint8_t string = 1;
  uint8_t affectedStrings = oldStrings^newStrings;
  const uint8_t channelswapped[] = {5,4,3,2,1,0};
  uint8_t channel;
  for(int i=0; i<STRING_COUNT; i++, string=string<<1) {
    if(string & affectedStrings) {
      channel = i;
      if (leftHanded) channel = channelswapped[i];
      stringIsMuted[channel] = (newStrings & string) ? true : false;
      if (stringIsMuted[channel]) {
        if (lastNote[channel].note != -1) {
          sendNoteOff(lastNote[channel].note, 0, channel+1);
          lastNote[channel].setNote(-1);
        }
      }
    }
  }
}

void setup() {
  for (int i=0; i<STRING_COUNT; i++) {
    pinMode(muteInput[i], INPUT);
    stringIsMuted[i] = false;
  }

  pinMode(LEFT_HANDED_SWITCH, INPUT_PULLUP);
  pinMode(ONE_CHANNEL_SWITCH, INPUT_PULLUP);
  
  MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

  MIDI.turnThruOff();

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
}

void loop() {
  static uint32_t oldTime = 0;
  uint32_t newTime = millis();
  static uint8_t oldStrings = 0;
  uint8_t newStrings = readStrings();

  if (newTime - oldTime >= 1000) {
    oldTime = newTime;
  }

  if (newStrings != oldStrings) {
    muteStrings(oldStrings, newStrings);
    oldStrings = newStrings;
  }

  leftHanded = digitalRead(LEFT_HANDED_SWITCH) == LOW;
  oneChannel = digitalRead(ONE_CHANNEL_SWITCH) == LOW;

  MIDI.read();
}
