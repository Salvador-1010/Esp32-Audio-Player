#include "power.h"
#include <Arduino.h>

//GPIO pins used for power sequence
const int Power_HLD = 25;
const int Power_BTN = 26; 
//ADC pin for reading the battery voltage 
const int batteryVoltageInput = 36;

//bool to track whether the device is in a state to be turned off (to ensure that it doesnt read the intial
//power on button press as a power off button press since they are the same button)
bool powerOffArmed = false;
//var to track the time elapased since the button was press/held
unsigned long timeButtonHeld;
bool isTiming = false;
//bool to track whether the powerOff sequence has been called already
bool poweringOff = false;


void powerSetup() {
    //sets the power hold pin to output so its output could be used to activate and deactivate the NPN transistor
    pinMode(Power_HLD, OUTPUT);
    //immediately writes it high to active the transistor and keep the power supplied even after button is released
    digitalWrite(Power_HLD, HIGH);
    //sets the button pullup high so that we can detect a button press when it gets pulled low
    pinMode(Power_BTN, INPUT_PULLUP);

    //gives the esp32 gpio adc pin the capabilitiy of reading voltages up to 2.1V accurately
    //(by default can only read up to 1.1V)
    analogSetPinAttenuation(batteryVoltageInput, ADC_11db);
}

void powerUpdate() {
    //reads button state for if logic
    int buttonState = digitalRead(Power_BTN);
    //when it detetcs the button was let go, the next long button press will power off the esp32
    if (buttonState == HIGH)
    {
        powerOffArmed = true;
        //sets the timing tracker back to false since the button is no longer being held down
        isTiming = false;
    }

    //when the button is pressed down 
    if (buttonState == LOW)
    {
        if (!isTiming)
        {
            timeButtonHeld = millis();
        }
        //sets the timing tracker to true when the button is pressed for the first time
        isTiming = true;

        //checks whether the button has been held longer than 2 seconds and the power off sequence hasnt already been started
        if ((millis() - timeButtonHeld >= 2000) && powerOffArmed && !poweringOff)
        {
            powerOff();
        }
    }
}

void powerOff() {
    //sets poweringoff sequence to true
    poweringOff = true;

    //will also eventually call proper shutdown sequencing functions
    digitalWrite(Power_HLD, LOW);
}

float getBatteryVoltage() {
    float batteryVoltage = analogReadMilliVolts(batteryVoltageInput) / 1000.0;
    //multiply it by 2 since the adc point reads half of the voltage due to the voltage divider
    return batteryVoltage * 2;
}