#pragma once

#include <Arduino.h>


//checks if the selected file is a valid music file
bool isValidFile(String newPath, String selectedSong);

//initializes the i2s and prepares hardware, allocates memory for audio buffers, maps physical pins, and powers the perhiperal
void setupI2S();

void testTone();


void audioSetup();
void audioUpdate();

void startSong();
void pauseToggle();

void setVolume(int newVolume);