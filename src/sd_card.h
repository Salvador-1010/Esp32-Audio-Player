#pragma once


bool sd_card_setup();
uint64_t get_free_bytes();
void eject_sd();

//function to navigate through directories
bool sd_card_list_directory(const char *path);