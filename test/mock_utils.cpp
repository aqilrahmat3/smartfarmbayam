#include "mock_utils.h"

#ifdef ARDUINO
  #include <Arduino.h>
  #include "firebase_utils.h"
  FirebaseData fbdo;
  LiquidCrystal_I2C lcd(0x27, 16, 2);
#else
  void* fbdo = nullptr;
  void* lcd = nullptr;
#endif

float mock_ph_value = 7.0;
float mock_tds_value = 500;
float mock_ec_value = 1.0;
float mock_temp_value = 25.0;

bool firebase_bool_sent = false;
String last_firebase_path = "";
bool last_firebase_val_bool = false;

float readPH() { return mock_ph_value; }
float readTDS() { return mock_tds_value; }
float readEC() { return mock_ec_value; }
float readTemperature() { return mock_temp_value; }

void setBool(const String& path, bool value) {
    firebase_bool_sent = true;
    last_firebase_path = path;
    last_firebase_val_bool = value;
}

float getFloat(String path) {
    if (path.find("ph_min") != String::npos) return 5.5;
    if (path.find("ph_max") != String::npos) return 6.5;
    return 0.5;
}
