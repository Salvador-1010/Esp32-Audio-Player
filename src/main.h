#pragma once
#include <Arduino.h>

void updateDisplay();

//string to take the current path and remove any excess absolute paths and/or "/"s
String formatCurrentPath(String path);

//backs out of the current directory to the previous one
void exitDirectory();

//enters a desired directory
void enterDirectory();

