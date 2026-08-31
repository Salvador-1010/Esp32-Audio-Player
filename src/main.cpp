#include <Arduino.h>

//includes sd_card set up files and functions
#include "sd_card.h"
//includes button and input control code
#include "controls.h"
//includes power sequencing function files
#include "power.h"
//incudes tft and ui files
#include "displayUI.h"

void setup() {
  //immedialty calls the power set up function to ensure the device keeps itself on (activites NPN transistor and PMOS)
  powerSetup();
  //gets all of the controls/buttons set up
  controlsSetup();

  Serial.begin(115200);

  //calls the sd card set up function
  sd_card_setup();
  
  test("/music");
  displaySetup();
  updateDisplay();
}

void loop() {
  powerUpdate();
  if (encoderValueChanged())
  {
    Serial.println(getEncoderValue());
  }
  drawDisplayMenu();
}

// void updateDisplay()
// {

// }