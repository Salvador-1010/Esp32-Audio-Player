//including SDMMC library to use SD card
#include <SD_MMC.h>
#include <Arduino.h>
#include "sd_card.h"

//defines the necessary pins for the built-in sd card reader
const int SD_MMC_CMD = 15; 
const int SD_MMC_CLK = 14;
const int SD_MMC_D0 = 2;

//the default first path is to the music folder
String currentPath = "/music";

bool sd_card_setup()
{
   //configure the GPIO pins used by the SDMMC interface
  if (!SD_MMC.setPins(SD_MMC_CLK,SD_MMC_CMD,SD_MMC_D0))
  {
    Serial.println("SD card pins failed to initialize.");
    return false;
  }
  //makes sure that the proper set up was succesful
  //since we are using the built in sd card reader we must set 1bitmode to true
  if (!SD_MMC.begin("/sdcard", true))
  {
    Serial.println("SD card Failed.");
    return false;
  }

  if (SD_MMC.cardType() == CARD_NONE)
  {
    Serial.println("No card inserted.");
    return false;
  }
  
  //if all the set up works then we can finalize initilization!
  //Serial.println("SD card initialized successfully!");
  return true;
  
} 

uint64_t get_free_bytes()
{
    uint64_t total_bytes = SD_MMC.totalBytes();
    uint64_t used_bytes = SD_MMC.usedBytes();

    return total_bytes - used_bytes;
}

void eject_sd()
{
    SD_MMC.end();
}

void test(const char *path)
{
  //tries to open the requested file path
    File file = SD_MMC.open(path);
    Serial.println(file.name());
    File entry = file.openNextFile();
    while(entry)
    {
      Serial.println(entry.name());
      entry.close();
      entry = file.openNextFile();
    }
}

std::vector<String> getFiles(const char *directory)
{
    //declares the string vector to store all of the song names
    std::vector<String> songs;
    //tries to open the requested file directory
    File file = SD_MMC.open(directory);
    File entry = file.openNextFile();
    //if entry is a proper file object then it will return 1
    while(entry)
    {
      songs.push_back(entry.name());
      entry.close();
      entry = file.openNextFile();
    }
    return songs;
}

String getCurrentDirectory()
{
  currentPath.remove(0,1);
  currentPath[0] = toupper(currentPath[0]);
  return currentPath;
}

