#include <Arduino.h>

//includes sd_card set up files and functions
#include "sd_card.h"
//includes button and input control code
#include "controls.h"


void setup() {
  Serial.begin(115200);

  if(!sd_card_setup())
  {
    return;
  }
  controlsSetup();
}

void loop() {
  if (encoderValueChanged())
  {
    Serial.println(getEncoderValue());
  }
}

