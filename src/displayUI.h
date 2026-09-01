#pragma once
#include <Arduino.h>
#include <vector>

void displaySetup();
void updateDisplay(int direction);
//expects a resizable vector of strings
//const.....& makes it so that an unchangable NON COPY of the vector is passed
void drawScreen(String title, const std::vector<String>& items);
void blinkCursor();
