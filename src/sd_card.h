#pragma once
#include <Arduino.h>

bool sd_card_setup();
uint64_t get_free_bytes();
void eject_sd();
String getDirectoryName();

//function to navigate through directories
void test(const char *path);