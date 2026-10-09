#pragma once

//enum to store the state of the button to return to main
enum buttonEvent
{
    NO_CLICK = 0,
    SINGLE_CLICK = 1,
    DOUBLE_CLICK = 2,
    DOUBLE_LONG_CLICK = 3,
    LONG_CLICK = 4,
    TRIPLE_CLICK = 5
};

void read_encoder_ISR();
void controlsSetup();
bool encoderValueChanged();

//returns either 1 for scrolling down or 0 for scrolling up
int getEncoderChangeDirection();

//returns false when the button is pressed (due to the pull up resistor it defaults to high)
bool encoderButtonPressed();

buttonEvent readButton1();
buttonEvent readRotaryBtn();

void controlsUpdate();