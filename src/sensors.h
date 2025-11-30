#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

float readTemperature();
float readPH();
float readTDS();
float readEC();
void initSensors();

#endif
