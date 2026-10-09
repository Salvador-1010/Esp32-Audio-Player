#pragma once

#include <Arduino.h>


//initializes the i2s and prepares hardware, allocates memory for audio buffers, maps physical pins, and powers the perhiperal
void setupI2S();

void testTone();


void audioSetup();
void audioUpdate();

void startSong(String songPath);
void pauseToggle();

void setVolume(int newVolume);

//helper debugging function to jsut get info on the audio buffer
void getBufferStatus();

//helper function that returns whether the song has ended (determined via evt_eof)
bool songEnded();

unsigned int getCurrentTime();

//helper function to give main +-15 sec seeking capabilities
void positionSeek(int seconds);