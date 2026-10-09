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
// bool encoderButtonState;
// bool previousEncoderButtonState;
// unsigned long encoderLastPressedAt;


//vars to keep track of the time since the last encoder value changed
unsigned long encoderLastChangedAt;


//stores the current path and the path we are trying to go to
//seperates the current default path, path to be added to move into a new dir, and the formatted version of the path for spelling
String currentPath = "/music";
String pathPrefix = currentPath + "/";
String formattedPath = currentPath;
//seperately stores the selected path in order to avoid changing the path when selecting a song
String selectedPath;

//stores the items in the current directory
std::vector<String> itemsList;

//stores the idx in the item list of the current song
int songidx = 0;

//determines whether we have toggled shuffle on or off
bool shuffleToggled = false;

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

//creates a playlist to store the current songs being listened to 
std::vector<String> activePlaylist;
String activeSongPath;
int activePlaylistSize = 0;

unsigned int currentSongTime = 0;


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
  itemsList = getFiles(currentPath.c_str(), "/music/");
  formattedPath = formatCurrentPath(currentPath);
  drawScreen(formattedPath, itemsList);
}

void loop() 
{

  powerUpdate();
  audioUpdate();
  controlsUpdate();

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

  //stores the value of the button function call to avoid calling it again
  buttonEvent button1Value = readButton1();

  //updates the encoder button state
  // bool currentReading = encoderButtonPressed();
  buttonEvent rotaryBtnValue = readRotaryBtn();

  //switch from manually rotary button handling to using button2 library
  // //checks to see if the previous state does not equal the current state
  // if (previousEncoderButtonState != currentReading)
  // {
  //   previousEncoderButtonState = currentReading;
  //   //updates the debounce timer
  //   encoderLastPressedAt = millis();
  // }

  //if the encoder button is pressed than it responds

  //no longer need manually button debounce checking
  // //checks to make sure its been stable long enough
  // if ((millis() - encoderLastPressedAt >= buttonPressDelay) && previousEncoderButtonState != encoderButtonState)
  // {
  //   //update the current stable encoder button value
  //   encoderButtonState = previousEncoderButtonState;
  //   //if the new button state is now low, then it was a button press
  //   if (encoderButtonState == LOW)
  //   {

  if (rotaryBtnValue != NO_CLICK)
  {
    //if the encoder is in browing mode then it will continue with the browsing logic 
    if (mode == BROWSING)
    {
      //first forms the desired path in a seperate var to check if it is a directory or song
      selectedPath = getSelectedItemName();
      //Serial.println(selectedPath);

      //then checks if the desired destination is a path or a folder
      // Serial.println(currentPath);
      // Serial.println(formattedPath);
      if (checkIfDirectory(selectedPath.c_str()))
      {
        //if it is a directory it updates the new path prefix 
        pathPrefix = selectedPath + "/";
        //if its a directory then updates the current path
        currentPath = selectedPath;
        //then formats the new path that we are in 
        formattedPath = formatCurrentPath(currentPath);

        enterDirectory();
        
        //TEMP CODE then prints all the songs in the directory
        // for (String item : itemsList)
        //   {
        //     Serial.println(item);
        //   }
      }
      else
      {
        activeSongPath = selectedPath;
        //updates the current active playlist 
        activePlaylist = itemsList;
        // Serial.println(selectedPath);
        startSong(activeSongPath);
        //updates the playlist we are "in" when a song is played (similar to spotify) so simply browsing another playlist one change the songs being shuffled
        activePlaylistSize = activePlaylist.size();
        //sets the songidx to that of the new song
        songidx = getSelectedItemIdx();
        // songidx = 0;
        // for (String item : itemsList)
        //   {
        //     Serial.printf("idx %i: ", songidx);
        //     songidx++;
        //     Serial.println(item);
        //   }
        // Serial.println(getSelectedItemIdx());
      }
    
    }
    //if not, then it is in audiocontrol mode so it continues with that logic
    else 
    {
      //Serial.println("PAUSE TOGGLE");
      pauseToggle();
    }
  }   
  

  //checks what button 1 needs us to do
  if (button1Value != NO_CLICK) // no click = 4 so if it is not 4 then we got a click
  {
    //then checks the mode we are in
    if (mode == BROWSING)
    {
      switch(button1Value)
      {
        case SINGLE_CLICK:
          exitDirectory();
          break;
        case DOUBLE_CLICK:
          break; //may later add functionality for double click in browsing mopde
      }
    }
    else if (mode == AUDIO_CONTROL)
    {
      switch(button1Value)
      {
        case SINGLE_CLICK:
          playNextSong();
          break;
        case DOUBLE_CLICK:
          previousSong();
          break;
        case LONG_CLICK:
          positionSeek(15); //skips ahead by 15 seconds
          break;
        case DOUBLE_LONG_CLICK:
          positionSeek(-15); //rewins by 15 seocnds
          break;
        case TRIPLE_CLICK:
          shuffleToggled = !shuffleToggled;
          Serial.println(shuffleToggled);
          break;
      }
      
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
      adjustVolume();
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

  //looping code to see if the song ended
  if (songEnded())
  {
    playNextSong();
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
  //Serial.println(currentPath);

  int index = currentPath.lastIndexOf("/");

  //erases the farthest back in order to effectively go back in the directory
  currentPath.remove(index, currentPath.length()-1);
  //Serial.println(currentPath);
  pathPrefix = currentPath + "/";

  //formats the path name to just the string
  formattedPath = formatCurrentPath(currentPath);

  //gets the new item list
  itemsList = getFiles(currentPath.c_str(), pathPrefix);
  //resets the navigating variables before switching
  resetNavigationState();
  //draws the new screen
  drawScreen(formattedPath, itemsList);
}

void enterDirectory()
{
    //gets the items inside that directory 
    itemsList = getFiles(currentPath.c_str(), pathPrefix);
    // for (String item : itemsList)
    //   {
    //     Serial.println(item);
    //   }
    // Serial.println(currentPath);
    // Serial.println(pathPrefix);
    resetNavigationState();
    drawScreen(formattedPath, itemsList);
}

void adjustVolume()
{
  //sets the last time it was changed to the current time
  encoderLastChangedAt = millis();
  //here will be the logic to change the volume 
  volume += getEncoderChangeDirection();
  //makes sure that the volume is not above 21 or below 0
  volume = constrain(volume, 0, 50);
  //Serial.print(volume);
  //then updates the volume
  setVolume(volume);
}

void playNextSong()
{
  //nothing to do if a song hasnt been played yet
  if (activePlaylistSize == 0)
  {
    return;
  }
  //first check to see if shuffle is toggled on or off
  if (shuffleToggled)
  {
    songidx = random(activePlaylistSize);
  }
  else //if its false then we just play the next song directly
  {
    //increment the song idx
    songidx++;

    // Serial.println(playlistSize);
    // Serial.println(songidx);
    //if the song index is too high then wraps around to the beginning
    if (songidx >= activePlaylistSize)
    {
      songidx = 0;
    }
  }

  Serial.println(songidx);
  //gets the song path at that new index
  activeSongPath = activePlaylist[songidx];
  startSong(activeSongPath);
  Serial.println(activePlaylistSize);
  Serial.println(activeSongPath);


  // for (String item : itemsList)
  // {
  //   Serial.println(item);
  // }
  //first increment the current song were at i think 
}

void previousSong()
{
  //nothing to do if a song hasnt been played yet
  if (activePlaylistSize == 0)
  {
    return;
  }

  currentSongTime = getCurrentTime();

  //if the song time is less than 2 seconds then it plays the previous track
  if (currentSongTime <= 2)
  {
    if (shuffleToggled)
    {
      //functionality if the shuffle is turned on
    }
    else
    {
      //sets the song idx to the previous song
      songidx--;
      //makes sure the idx doesnt go below 0
      if (songidx < 0)
      {
        songidx = activePlaylistSize - 1;
      }
    }

    //then sets the active song path 
    activeSongPath = activePlaylist[songidx];
  }
  startSong(activeSongPath);
}