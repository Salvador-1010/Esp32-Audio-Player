#pragma once
#include <Arduino.h>
#include <vector>

void displaySetup();
void updateDisplay(int direction=0);
//expects a resizable vector of strings
//const.....& makes it so that an unchangable NON COPY of the vector is passed
void drawScreen(String title, const std::vector<String>& list);
void blinkCursor();


//function that gives the name of current selected item 
String getSelectedItemName();

//resets the cursor to the inital starting position and also clears the old cursor 
void resetCursor();

//only resets the navigation state vars 
void resetNavigationState();