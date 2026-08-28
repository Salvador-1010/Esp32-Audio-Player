#include <Arduino.h>

//includes sd_card set up files and functions
#include "sd_card.h"
//includes button and input control code
#include "controls.h"

//temporary test code to configure power latching system
const int Power_HLD = 25;
const int Power_BTN = 26; 

void setup() {
  //sets the power hold pin to output so its output could be used to activate and deactivate the NPN transistor
  pinMode(Power_HLD, OUTPUT);
  //immediately writes it high to active the transistor and keep the power supplied even after button is released
  digitalWrite(Power_HLD, HIGH);

  //sets the button pullup high so that we can detect a button press when it gets pulled low
  pinMode(Power_BTN, INPUT_PULLUP);
  Serial.begin(115200);

  // if(!sd_card_setup())
  // {
  //   return;
  // }
  // controlsSetup();
}

void loop() {
  // if (encoderValueChanged())
  // {
  //   Serial.println(getEncoderValue());
  // }
}

