#include <Arduino.h>
#include <string>

//includes sd_card set up files and functions
#include "sd_card.h"
//includes button and input control code
#include "controls.h"
//includes power sequencing function files
#include "power.h"
//incudes tft and ui files
#include "displayUI.h"
//includes main files
#include "main.h"
//includes audio player functions
#include "audioPlayer.h"
//includes preferences library to access esp32s NVS (non volatile storage)
#include <Preferences.h>

//makes sure that button presses arent read many times over so create a debounce delay
unsigned long buttonPressDelay = 50;
unsigned long encoderPressedAt;
bool encoderPressed = false; 

//vars to keep track of button1 being pressed
bool button1Pressed = false;
unsigned long button1PressedAt;

//vars to keep track of the time since the last encoder value changed
unsigned long encoderLastChangedAt;

//stores the current path and the path we are trying to go to
//seperates the current default path, path to be added to move into a new dir, and the formatted version of the path for spelling
String currentPath = "/music";
String additionalPath;
String formattedPath = currentPath;
//seperately stores the selected path in order to avoid changing the path when selecting a song
String selectedPath;

//stores the items in the current directory
std::vector<String> itemsList;

//enum to store the current mode state
enum playerMode {
  BROWSING,
  AUDIO_CONTROL
};

//defaults the mode state to browing
playerMode mode = BROWSING;

//creates preferences object to store volume in NVS
Preferences preference;
//stores the current volume of the player
int savedVolume;
int volume;

void setup() {
  //immedialty calls the power set up function to ensure the device keeps itself on (activites NPN transistor and PMOS)
  powerSetup();
  //gets all of the controls/buttons set up
  controlsSetup();

  Serial.begin(115200);

  //calls the sd card set up function
  sd_card_setup();

  //sets up the display
  displaySetup();

  audioSetup();

  //sets up the preference namespace
  preference.begin("audio", false);
  //sets the current volume to whatever was previously stored
  //if there was no stored valued then defaults to 10
  volume = preference.getInt("volume", 10);
  //then saves that volume as the lastest NVS volume
  savedVolume = volume;
  //then sets the volume
  setVolume(volume);

  // //sets up the i2s
  // setupI2S();

  
  //testing the get directory funciton
  //converts string to char*
  itemsList = getFiles(currentPath.c_str());
  formattedPath = formatCurrentPath(currentPath);
  drawScreen(formattedPath, itemsList);
}

void loop() {
  powerUpdate();
  audioUpdate();

  //cursor always blinks no matter what
  blinkCursor();
  
  //first updates the mode state if necessary
  if (getModeChange())
  {
    if (mode == AUDIO_CONTROL)
    {
      mode = BROWSING;
    }
    else
    {
      mode = AUDIO_CONTROL;
    }
  }

  //if the encoder button is pressed than it responds
  //(checks for ! because it is default high due to pullup resistor)
  if (!encoderButtonPressed())
  {
    //makes sure that button press function reaction is only called once
    if (!encoderPressed)
    {
      encoderPressed = true;
      encoderPressedAt = millis();

      //if the encoder is in browing mode then it will continue with the browsing logic 
      if (mode == BROWSING)
      {
        //first forms the desired path in a seperate var to check if it is a directory or song
        selectedPath = currentPath + "/" + getSelectedItemName();
        //then checks if the desired destination is a path or a folder
        // Serial.println(currentPath);
        // Serial.println(formattedPath);
        if (checkIfDirectory(selectedPath.c_str()))
        {
          //if its a directory then updates the current path
          currentPath = selectedPath;
          //then formats the new path that we are in 
          formattedPath = formatCurrentPath(currentPath);

          enterDirectory();
        }
        else
        {
          //if it is not a directory than the current path stays the same but the selected path still includes the song name

          //now since we know it is a file we have to check if its a playable file (.wav, .mp3, or .flac)
          if(isValidFile(selectedPath, getSelectedItemName()))
          {
            //if the file is valid then the path will be saved and we can play the song
            startSong();
          }
        }
        Serial.println(selectedPath);
      }
      //if not, then it is in audiocontrol mode so it continues with that logic
      else 
      {
        pauseToggle();
      }

    }
  }
  else if (millis() - encoderPressedAt > buttonPressDelay)
  {
    encoderPressed = false;
  }

  //logic that runs when button1 (back/something else button)
  if (readButton1())
  {
    //returns true for when it is pressed
    if (!button1Pressed)
    {
      button1Pressed = true;
      button1PressedAt = millis();

      //will eventually add logic to check what mode the player is in

      exitDirectory();
    }
    else if (millis() - button1PressedAt > buttonPressDelay)
    {
      button1Pressed = false;
    }
  }
  
  //if the encoder scrolls then it adjust for that
  if (encoderValueChanged())
  {
    //once again, checks the mode we are in
    if (mode == BROWSING)
    {
      //if the encoder value changed then were gonna update the display to change cursor and selected item
      updateDisplay(getEncoderChangeDirection());
    }
    //else, its in audio control mode so it changes the volume
    else
    {
      //sets the last time it was changed to the current time
      encoderLastChangedAt = millis();
      //here will be the logic to change the volume 
      volume += getEncoderChangeDirection();
      //makes sure that the volume is not above 21 or below 0
      volume = constrain(volume, 0, 21);
      //then updates the volume
      setVolume(volume);
    }
  }
  //if the last volume != current volume AND the encoder hasnt been changed in 500ms, write to flash
  if ((savedVolume != volume) && (millis() - encoderLastChangedAt >= 500))
  {
    //writes the newvolume to memory
    preference.putInt("volume", volume);
    //then saves that as the lastest NVS volume value 
    savedVolume = volume;
  }
  
}

String formatCurrentPath(String path)
{
  //searches for the last "/" appearing in the string
  int lastIndex = path.lastIndexOf("/");
  path.remove(0, lastIndex + 1);
  path[0] = toupper(path[0]);
  return path;
}

void exitDirectory()
{
  //checks to see if the user is already in the farthest back dir, if so they cant go back so it just returns
  if (currentPath == "/music")
  {
    return;
  }

  int index = currentPath.lastIndexOf("/");

  //erases the farthest back in order to effectively go back in the directory
  currentPath.remove(index, currentPath.length()-1);

  //formats the path name to just the string
  formattedPath = formatCurrentPath(currentPath);

  //gets the new item list
  itemsList = getFiles(currentPath.c_str());
  //resets the navigating variables before switching
  resetNavigationState();
  //draws the new screen
  drawScreen(formattedPath, itemsList);
}

void enterDirectory()
{
    //gets the items inside that directory 
    itemsList = getFiles(currentPath.c_str());
    resetNavigationState();
    drawScreen(formattedPath, itemsList);
}

