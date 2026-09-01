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
  

  displaySetup();
  //testing the get directory funciton
  std::vector<String> songs = getFiles("/music");

  String test = getCurrentDirectory();
  
  drawScreen(test, songs);
}

void loop() {
  powerUpdate();

  //cursor always blinks no matter what
  blinkCursor();

  if (encoderValueChanged())
  {
    //if the encoder value changed then were gonna update the display to change cursor and selected item
    updateDisplay(getEncoderChangeDirection());
  }
}

// void updateDisplay()
// {

// }