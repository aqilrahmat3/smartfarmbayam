#include <Arduino.h>
#include <WiFi.h>

#include "sensors.h"
#include "firebase_utils.h"
#include "display.h"
#include "manual_mode.h"
#include "auto_mode.h"
#include "alert_manager.h"

#define LAMP_PIN 5
#define PUMP_ACID_PIN 2
#define PUMP_NUTRI_PIN 4

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

void setup() {
  Serial.begin(115200);

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(PUMP_ACID_PIN, OUTPUT);
  pinMode(PUMP_NUTRI_PIN, OUTPUT);

  digitalWrite(LAMP_PIN, LOW);
  digitalWrite(PUMP_ACID_PIN, LOW);
  digitalWrite(PUMP_NUTRI_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status()!=WL_CONNECTED) delay(200);

  initSensors();
  initDisplay();
  initFirebase();
}

void loop() {

  Firebase.getBool(fbdo, "/app/config/thresholds/is_manual");
  bool isManual = fbdo.boolData();
 
  if (isManual) {
    // MODE MANUAL
    handleManualMode();
  } else {
    // MODE AUTO
    handleAutoMode();  
  }

  delay(500);
  float temp = readTemperature();
  float ph = readPH();
  float tds = readTDS();
  float ec = readEC();
  int lightAnalog = readLightAnalog();
  String cat = getLightCategory(lightAnalog);
  showStatus(ph, tds, ec, temp, cat, isManual);
  sendToFirebase(ph, tds, ec, temp, cat);
  checkAlerts(ph, tds, ec, temp);

  delay(2000);
}
