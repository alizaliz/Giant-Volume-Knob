#include <HID-Project.h>
#include <HID-Settings.h>

#include <Encoder.h>

/* Components */
Encoder knobVol(2, 3);  // VOLUME KNOB
const int buttonMute = 4; // MUTE BUTTON
const int buttonSource = 5; // SOUND SWITCH SHORTCUT BUTTON
const int switchMain = 6; // MAIN TOGGLE SWITCH 

/* Variables */
boolean hidRunning = false;
long volValue  = -999; 

/* Input States */
int stateMain = 0;
int stateSource = 0;
int stateVol = 0;

/* Initialise Components */
void setup() {
  pinMode(buttonMute, INPUT);
  pinMode(buttonSource, INPUT);
  pinMode(switchMain, INPUT);
}



void loop() {
  stateMain = digitalRead(switchMain); // Check to see if HID should be running


  if (stateMain == HIGH) { 

    /* Toggle on the HID */
    if (!hidRunning) {
      Consumer.begin();
      hidRunning = true;
    }

    /* React to changes of encoder */
    long newVolValue;  
    newVolValue = knobVol.read();
    if (newVolValue != volValue) { // Changed position
      if (newVolValue > volValue) { // Right turn
        Consumer.write(MEDIA_VOLUME_DOWN);

      } else if (newVolValue < volValue) { // Left turn
        Consumer.write(MEDIA_VOLUME_UP);
      }
      volValue = newVolValue;
    }

    /* Simple button press for knob momentary switch */
    stateVol = digitalRead(buttonMute);
    if (stateVol == HIGH) { 
      Consumer.write(MEDIA_VOLUME_MUTE);
    }

    /* Send key combination for Sound Switch application */
    stateSource = digitalRead(buttonSource);
    if (stateSource == HIGH) { 
      Keyboard.press(HID_KEYBOARD_RIGHT_CONTROL);
      Keyboard.press(HID_KEYBOARD_RIGHT_ALT);
      Keyboard.press(HID_KEYBOARD_F11);
      Keyboard.releaseAll();
    }

    
    delay(100); // Simple debounce
    
  } else {

    /* Toggle off the HID */
    if (hidRunning) { 
      Consumer.end();
      hidRunning = false;
    }

  }
}

