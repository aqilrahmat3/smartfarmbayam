#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

float readTemperature();
float readPH();
float readTDS();
float readEC();
String getLightCategory(int value);
int readLightAnalog();

void initSensors();

#endif
