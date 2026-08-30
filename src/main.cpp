#include <Arduino.h>

//includes sd_card set up files and functions
#include "sd_card.h"
//includes button and input control code
#include "controls.h"
//includes power sequencing function files
#include "power.h"

void setup() {
  powerSetup();
  Serial.begin(115200);

  // if(!sd_card_setup())
  // {
  //   return;
  // }
  // controlsSetup();
}

void loop() {
  powerUpdate();
  // if (encoderValueChanged())
  // {
  //   Serial.println(getEncoderValue());
  // }
  Serial.println(getBatteryVoltage());
}

