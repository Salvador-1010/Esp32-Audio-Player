#include <Arduino.h>

#include "controls.h"

//defining the rotary encoder pins
const int rotary_clk = 39;
const int rotary_DT = 36;
//the rotation pins can use 36 and 39 because of the onboard KW-040 pull up resistors
//but the push button pin needs to use the esp32 pull up resistors
const int rotary_SW = 21;

//defines as a volatile int because it may change outside of the normal program flow
volatile int encoderPos = 0;
int lastEncoderPos = 0;

//since transitions increment by +-2, adding a separate variable is an easy fix to making sure encoderPos increments by +-1
volatile int transitionAccumulator = 0;

//tracks the previous state combination of clk and DT
static volatile uint8_t lastState = 0;

//stores the direction that the encoder traveled in the last change (1 for down -1 for up)
int encoderChangeDirection;

//set up function that gets all the pins and functions ready
void controlsSetup() {
    pinMode(rotary_clk, INPUT);
    pinMode(rotary_DT, INPUT);
    //again, push pin is connected to an esp32 built in pull up resistor pin
    pinMode(rotary_SW, INPUT_PULLUP);

    //reads the intial state of the encoder
    //creates a 2bit binary number with the first 0/1 being the roatary clk value and the second being the rotary dt value
    //"<<" moves the digit over by 1 value  
    lastState = (digitalRead(rotary_clk) << 1) | digitalRead(rotary_DT);

    //triggers interrupt when either pin changes value
    attachInterrupt(digitalPinToInterrupt(rotary_clk), read_encoder_ISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rotary_DT), read_encoder_ISR, CHANGE);
}


void IRAM_ATTR read_encoder_ISR() 
{
    //defines variable to track the current state of the encoder
    //NOTE: Single "|" is a bitwise OR rather than an operational OR
    uint8_t currentState = digitalRead(rotary_clk) << 1 | digitalRead(rotary_DT);

    //defines pair variable to keep track of valid and non valid transiitions
    uint8_t statePair = (lastState << 2) | currentState;

    //since there are specific valid transition pairs we can use that for accepting increments
    if (statePair == 0b0010 || statePair == 0b1011 || statePair == 0b1101 || statePair == 0b0100)
    {
        transitionAccumulator++;
    }
    else if (statePair == 0b0001 || statePair == 0b0111 || statePair == 0b1110 || statePair == 0b1000)
    {
        transitionAccumulator--;
    }

    //runs code to increment encoderPos
    if (transitionAccumulator == 2)
    {
        //sets the transitionaccumlator back to 0 to continue tracking
        transitionAccumulator = 0;
        //updates the current value
        encoderPos++;
    }
    else if (transitionAccumulator == -2)
    {
        transitionAccumulator = 0;
        encoderPos--;
    }

    lastState = currentState;    
}

bool encoderValueChanged() 
{
    //checks to see if the current pos is equal to the last known position and if not then the value has changed and it sets the new value
    if (encoderPos != lastEncoderPos)
    {
        encoderChangeDirection = encoderPos - lastEncoderPos;
        lastEncoderPos = encoderPos;
        return true;
    }

    return false;
}

//helper function for other files to get the direction the user scrolled depeneding on the actual encoder value
int getEncoderChangeDirection()
{
    return encoderChangeDirection;
}