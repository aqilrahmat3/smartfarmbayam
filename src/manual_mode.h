#pragma once
#include <Arduino.h>
#include "firebase_utils.h"

void handleManualMode() {
  bool acidState = getBool("/devices/tool/pump_acid");
  bool nutriState = getBool("/devices/tool/pump_nutrient");

  digitalWrite(2, acidState ? HIGH : LOW);
  digitalWrite(4, nutriState ? HIGH : LOW);
}
