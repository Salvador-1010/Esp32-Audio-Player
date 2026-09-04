#pragma once

#include <Arduino.h>


//checks if the selected file is a valid music file
bool isValidFile(String selected);

//initializes the i2s and prepares hardware, allocates memory for audio buffers, maps physical pins, and powers the perhiperal
void setupI2S();