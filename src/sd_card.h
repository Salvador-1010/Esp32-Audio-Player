#pragma once
#include <Arduino.h>
#include <vector>

bool sd_card_setup();
uint64_t get_free_bytes();
void eject_sd();
String getCurrentRoot();

//function to navigate through directories
//uses the standard library logic of vector and creates an resizable list of strings
std::vector<String> getFiles(const char *directory);

//function to respond to rotary click and open either the file or directory selected
