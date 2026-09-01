#pragma once

void IRAM_ATTR read_encoder_ISR();
void controlsSetup();
bool encoderValueChanged();

//returns either 1 for scrolling down or 0 for scrolling up
int getEncoderChangeDirection();