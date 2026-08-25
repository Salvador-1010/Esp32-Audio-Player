#pragma once

void IRAM_ATTR read_encoder_ISR();
void controlsSetup();
bool encoderValueChanged();
int getEncoderValue();