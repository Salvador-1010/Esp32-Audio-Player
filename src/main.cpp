#include <Arduino.h>

//includes sd_card set up files and functions
#include "sd_card.h"

#include <SD_MMC.h>

void setup() {
  Serial.begin(115200);

  if(!sd_card_setup())
  {
    return;
  }
  SD_MMC.open("/");
}

void loop() {
  
}

