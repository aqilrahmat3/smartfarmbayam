#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void initDisplay();
void showStatus(float ph, float tds, float ec, float temp,bool isManual);

#endif
