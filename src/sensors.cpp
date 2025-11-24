#include "sensors.h"
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define PH_PIN 33
#define TDS_PIN 32
#define EC_PIN 35
#define LDR_AO 34

void initSensors() {
  dht.begin();
}

float readTemperature() {
  return dht.readTemperature();
}

float readPH() {
  int rawPh = analogRead(PH_PIN);
  return (rawPh / 4095.0) * 14.0;
}

float readTDS() {
  int rawTDS = analogRead(TDS_PIN);
  return map(rawTDS, 0, 4095, 0, 2000);
}

float readEC() {
  int rawEC = analogRead(EC_PIN);
  return map(rawEC, 0, 4095, 0, 3);
}

int readLightAnalog() {
  return analogRead(LDR_AO);
}

String getLightCategory(int value) {
  if(value<800) return "Gelap";
  else if(value<1600) return "Redup";
  else if(value<2600) return "Normal";
  else if(value<3500) return "Terang";
  else return "SgtTerang";
}
