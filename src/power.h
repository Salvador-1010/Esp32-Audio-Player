//code file to manage power-related functionality such as powering on/off and monitoring power status

#pragma once

//functions to control the on/off status of the entire device
void powerSetup();
void powerUpdate();
void powerOff();

//function to read the current battery voltage
float getBatteryVoltage();


