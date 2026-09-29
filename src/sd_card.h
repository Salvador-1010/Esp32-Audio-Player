#pragma once
#include <Arduino.h>
#include <vector>

bool sd_card_setup();
uint64_t get_free_bytes();
void eject_sd();
String getCurrentRoot();

//function to navigate through directories
//uses the standard library logic of vector and creates an resizable list of strings
//added optional string path parameter to make sure that all of the files include the absolute path rather than just the file name
std::vector<String> getFiles(const char *directory, String path = "");

//function to check if the selected item is either a file or directory
bool checkIfDirectory(const char* currentPath);
