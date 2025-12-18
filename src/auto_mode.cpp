#include "auto_mode.h"

#ifdef ARDUINO

#include <Arduino.h>
#include "firebase_utils.h"
#include "sensor.h"

// ===== KONSTANTA =====
const float PH_MAX = 6.5;
const float PH_MIN = 5.5;

const float TDS_MIN = 800;
const float TDS_MAX = 1700;

const float EC_MIN = 1.8;
const float EC_MAX = 2.5;

// ===== TIMER =====
unsigned long lastAcidRun = 0;
unsigned long lastNutrientRun = 0;

const unsigned long MAX_RUN_TIME = 12000;
const unsigned long COOLDOWN_TIME = 45000;

void initAutoPins() {
    pinMode(RELAY_PUMP_ACID, OUTPUT);
    pinMode(RELAY_PUMP_NUTRI, OUTPUT);

    digitalWrite(RELAY_PUMP_ACID, LOW);
    digitalWrite(RELAY_PUMP_NUTRI, LOW);
}

void handleAutoMode() {
    float pHValue  = readPH();
    float ecValue  = readEC();
    float tdsValue = readTDS();

    unsigned long now = millis();

    if (pHValue > PH_MAX && (now - lastAcidRun > COOLDOWN_TIME)) {
        digitalWrite(RELAY_PUMP_ACID, HIGH);
        lastAcidRun = now;
        setBool("/devices/tool/pump_acid", true);
    } 
    else if (pHValue < PH_MIN || (now - lastAcidRun > MAX_RUN_TIME)) {
        digitalWrite(RELAY_PUMP_ACID, LOW);
        setBool("/devices/tool/pump_acid", false);
    }

    bool nutrisiKurang = (tdsValue < TDS_MIN || ecValue < EC_MIN);
    bool nutrisiTinggi = (tdsValue > TDS_MAX || ecValue > EC_MAX);

    if (nutrisiKurang && (now - lastNutrientRun > COOLDOWN_TIME)) {
        digitalWrite(RELAY_PUMP_NUTRI, HIGH);
        lastNutrientRun = now;
        setBool("/devices/tool/pump_nutrient", true);
    } 
    else if (nutrisiTinggi || (now - lastNutrientRun > MAX_RUN_TIME)) {
        digitalWrite(RELAY_PUMP_NUTRI, LOW);
        setBool("/devices/tool/pump_nutrient", false);
    }
}

#endif // ARDUINO
