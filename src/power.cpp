#include "power.h"
#include <Arduino.h>

//GPIO pins used for power sequence
const int Power_HLD = 25;
const int Power_BTN = 26; 

void powerSetup() {
    //sets the power hold pin Sto output so its output could be used to activate and deactivate the NPN transistor
    pinMode(Power_HLD, OUTPUT);
    //immediately writes it high to active the transistor and keep the power supplied even after button is released
    digitalWrite(Power_HLD, HIGH);

    //sets the button pullup high so that we can detect a button press when it gets pulled low
    pinMode(Power_BTN, INPUT_PULLUP);
}

void powerUpdate() {
}

void powerOff() {
}
