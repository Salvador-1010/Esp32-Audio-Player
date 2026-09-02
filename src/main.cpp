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

//makes sure that button presses arent read many times over so create a debounce delay
unsigned long buttonPressDelay = 500;
unsigned long encoderPressedAt;
bool encoderPressed = false; 

//vars to keep track of button1 being pressed
bool button1Pressed = false;
unsigned long button1PressedAt;

//stores the current path and the path we are trying to go to
//seperates the current default path, path to be added to move into a new dir, and the formatted version of the path for spelling
String currentPath = "/music";
String additionalPath;
String formattedPath = currentPath;

//stores the items in the current directory
std::vector<String> itemsList;

String test;

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
  //converts string to char*
  itemsList = getFiles(currentPath.c_str());
  formattedPath = formatCurrentPath(currentPath);
  drawScreen(formattedPath, itemsList);
}

void loop() {
  powerUpdate();

  //cursor always blinks no matter what
  blinkCursor();

  //if the encoder button is pressed than it responds
  //(checks for ! because it is default high due to pullup resistor)
  if (!encoderButtonPressed())
  {
    //makes sure that button press function reaction is only called once
    if (!encoderPressed)
    {
      encoderPressed = true;
      encoderPressedAt = millis();
      //eventually it will check what mode the player is in but for now it can only be in navigate mode

      enterDirectory();
    }
  }
  else if (millis() - encoderPressedAt > buttonPressDelay)
  {
    encoderPressed = false;
  }

  //logic that runs when button1 (back/something else button)
  if (readButton1())
  {//returns true for when it is pressed
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
    //if the encoder value changed then were gonna update the display to change cursor and selected item
    updateDisplay(getEncoderChangeDirection());
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
    //first forms the desired path from the current path and the next path to add on that was selected
    currentPath = currentPath + "/" + getSelectedItemName();
    formattedPath = formatCurrentPath(currentPath);
    //gets the items inside that directory 
    itemsList = getFiles(currentPath.c_str());
    resetNavigationState();
    drawScreen(formattedPath, itemsList);
}
