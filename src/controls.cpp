#include <Arduino.h>

#include "controls.h"
#include "Button2.h"

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

//stores button1 value (true for pressed false for not pressed)
bool button1Value = false;

// //vars to keep track of button1 being pressed
// bool button1Pressed = false;
// unsigned long button1PressedAt;
// int buttonPressDelay = 50;

const int button1Pin = 13;
Button2 button1;
buttonEvent button1State;


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

    //sets it to pulldown so it defaults to 0 when not pressed
    pinMode(button1Pin, INPUT_PULLDOWN);

    //setup functionality for back/next/previous track button
    button1.begin(button1Pin, INPUT_PULLDOWN, false);
}

void controlsUpdate()
{
    button1.loop();
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
        transitionAccumulator = transitionAccumulator + 1;
    }
    else if (statePair == 0b0001 || statePair == 0b0111 || statePair == 0b1110 || statePair == 0b1000)
    {
        transitionAccumulator = transitionAccumulator - 1;
    }

    //runs code to increment encoderPos
    if (transitionAccumulator == 2)
    {
        //sets the transitionaccumlator back to 0 to continue tracking
        transitionAccumulator = 0;
        //updates the current value
        encoderPos = encoderPos + 1;
    }
    else if (transitionAccumulator == -2)
    {
        transitionAccumulator = 0;
        encoderPos = encoderPos - 1;
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

bool encoderButtonPressed()
{
    return digitalRead(rotary_SW);
}

buttonEvent readButton1()
{

    if (button1.wasPressed())
    {
        clickType click = button1.read();
        //sets the button1state to the click
        if (click == single_click)
        {
            button1State = SINGLE_CLICK;
        }
        else if (click == double_click)
        {
            button1State = DOUBLE_CLICK;
        }
        else //if its something like triple or long click for now just default to no click
        {
            button1State = NO_CLICK;
        }
    }
    else
    {
        //by default button1State is no click
        button1State = NO_CLICK;
    }
    return button1State;
}