#pragma once

void IRAM_ATTR read_encoder_ISR();
void controlsSetup();
bool encoderValueChanged();

//returns either 1 for scrolling down or 0 for scrolling up
int getEncoderChangeDirection();

//returns false when the button is pressed (due to the pull up resistor it defaults to high)
bool encoderButtonPressed();