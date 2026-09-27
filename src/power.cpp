#include "power.h"
#include <Arduino.h>

//GPIO pins used for power sequence
const int Power_HLD = 25;
const int Power_BTN = 26; 
//ADC pin for reading the battery voltage 
const int batteryVoltageInput = 34;

//bool to track whether the device is in a state to be turned off (to ensure that it doesnt read the intial
//power on button press as a power off button press since they are the same button)
bool powerOffArmed = false;
//var to track the time elapased since the button was press/held
unsigned long buttonPressedAt;
unsigned long timeButtonHeld;
bool isTiming = false;
//bool to track whether the powerOff sequence has been called already
bool poweringOff = false;
bool previousButtonState;
bool buttonState;
//bool to store whether the mode has changed 
bool modeChangeRequested;



void powerSetup() {
    //sets the power hold pin to output so its output could be used to activate and deactivate the NPN transistor
    pinMode(Power_HLD, OUTPUT);
    //immediately writes it high to active the transistor and keep the power supplied even after button is released
    digitalWrite(Power_HLD, HIGH);
    //sets the button pullup high so that we can detect a button press when it gets pulled low
    pinMode(Power_BTN, INPUT_PULLUP);
    buttonState = digitalRead(Power_BTN);

    //gives the esp32 gpio adc pin the capabilitiy of reading voltages up to 2.1V accurately
    //(by default can only read up to 1.1V)
    analogSetPinAttenuation(batteryVoltageInput, ADC_11db);
    // Serial.print("Power has been set up, Time: ");
    // Serial.println(millis());
}

void powerUpdate() {
    //saves the button state before its updaded
    previousButtonState = buttonState;
    //reads button state for if logic
    buttonState = digitalRead(Power_BTN);

    //when it detetcs the button was let go, the next long button press will power off the esp32
    if (buttonState)
    {
        powerOffArmed = true;
        //sets the timing tracker back to false since the button is no longer being held down
        isTiming = false;
        // Serial.print("Button let go, Time: ");
        // Serial.println(millis());


    }

    //when the button is pressed down 
    if (!buttonState)
    {
        // Serial.print("Button Pressed, Time: ");
        // Serial.println(millis());
        if (!isTiming)
        {   
            //starts to time when the button was pressed at if not previously being timed
            buttonPressedAt = millis();
            // Serial.print("time elasped started, Time: ");
            // Serial.println(millis());
        }
        //sets the timing tracker to true when the button is pressed for the first time
        isTiming = true;

        //saves the time the button was held down for
        timeButtonHeld = millis() - buttonPressedAt;

        //checks whether the button has been held longer than 2 seconds and the power off sequence hasnt already been started
        if ((timeButtonHeld >= 2000) && powerOffArmed && !poweringOff)
        {
            // Serial.print("Power off function called, Time: ");
            // Serial.println(millis());
            powerOff(); 
        }
    }

    //checks to see if the button was released
    if (!previousButtonState && buttonState)
    {
        //after the button has been released, it checks how long it was held down for
        //checks to see if it was less than 2000 but above a debounce threshold to change modes
        if (timeButtonHeld >= 30 && timeButtonHeld < 2000)
        {   
            //if the button was just released and it was in the right interval the mode is meant to change
            modeChangeRequested = true;
            //sets time button held back to 0
            timeButtonHeld = 0;
            return;
        }
    }
  
}

void powerOff() {
    // Serial.print("Power off function ran, Time: ");
    // Serial.println(millis());
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

bool getModeChange()
{
    //saves the current mode state
    bool modeState = modeChangeRequested;
    //resets the request state
    modeChangeRequested = false;
    //returns what the mode state was 
    return modeState;
}