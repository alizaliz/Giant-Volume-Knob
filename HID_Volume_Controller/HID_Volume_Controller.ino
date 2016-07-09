/**
 *  License: GNU GENERAL PUBLIC LICENSE
 *  Libraries: NicoHood HID, Teensy Encoder Library
 * 
 *  Arduino ATmega32u4 class to control PC system volume (including mute) 
 *  with a rotary encoder and playback devices with a push button.
 *  N.B. Cycling playback devices is achieved by sending shortcut key strokes
 *  for soundSwitch application. 
 *  
 *  Sound Switch application by Antoine Aflalo
 *  https://github.com/Belphemur/SoundSwitch
 *  https://www.aaflalo.me/downloads/
 *  
 */

#include <HID-Project.h>
#include <HID-Settings.h>

#include <Encoder.h>

/* Components */
Encoder knobVol(2, 3);  // VOLUME KNOB
const int buttonSource = 4; // SOUND SWITCH SHORTCUT BUTTON
const int buttonMute = 5; // MUTE BUTTON
const int switchMain = 6; // MAIN TOGGLE SWITCH 

/* Variables */
bool hidRunning = false;
long volValue  = -999; 

/* Input States */
int stateMain = 0;
int stateSource = 0;
int stateVol = 0;

/* Button already held down? - For single press button rather than hold switch */
bool downSource = false;
bool downVol = false;


/* Initialise Components */
void setup() {
  pinMode(buttonMute, INPUT);
  pinMode(buttonSource, INPUT);
  pinMode(switchMain, INPUT);
}



void loop() {
  /* Check to see if HID should be running
  * Allows (easier) programming of board when switched off
  */
  stateMain = digitalRead(switchMain);

  
  if (stateMain == HIGH) { //Main Jumper/Switch

    /* Toggle on the HID */
    if (!hidRunning) {
      Consumer.begin();
      hidRunning = true;
    }

    /* Update button states */
    stateSource = digitalRead(buttonSource);
    stateVol = digitalRead(buttonMute);
    
    /* Button is released */
    if(downSource && stateSource == LOW){downSource = false;}
    if(downVol && stateVol == LOW){downVol = false;}

    /* Simple button press for knob momentary switch */
    if (!downVol && stateVol == HIGH) { 
      Consumer.write(MEDIA_VOLUME_MUTE);
      downVol = true; // Only send command once per press
    }

    /* Send key combination for Sound Switch application */
    if (!downSource && stateSource == HIGH) { 
      Keyboard.press(HID_KEYBOARD_RIGHT_CONTROL);
      Keyboard.press(HID_KEYBOARD_RIGHT_ALT);
      Keyboard.press(HID_KEYBOARD_F11);
      Keyboard.releaseAll();
      downSource = true; // Only send command once per press
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


    delay(100); // Simple debounce
    
  } else {

    /* Toggle off the HID */
    if (hidRunning) { 
      Consumer.end();
      hidRunning = false;
    }

  }
}

