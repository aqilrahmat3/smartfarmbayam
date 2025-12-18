#pragma once

#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <string>
  using String = std::string;
#endif

// ---- Mock sensor values ----
extern float mock_ph_value;
extern float mock_tds_value;
extern float mock_ec_value;
extern float mock_temp_value;

// ---- Firebase mock state ----
extern bool firebase_bool_sent;
extern String last_firebase_path;
extern bool last_firebase_val_bool;

// ---- Mock functions ----
float readPH();
float readTDS();
float readEC();
float readTemperature();

void setBool(const String& path, bool value);
float getFloat(String path);

// ---- Dummy declarations ----
extern void* fbdo;
extern void* lcd;
