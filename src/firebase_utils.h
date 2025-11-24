#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include <Arduino.h>
#include <FirebaseESP32.h>

void initFirebase();
void sendToFirebase(float ph, float tds, float ec, float temp, String lightCat);

bool getBool(const String& path);
float getFloat(const String& path);

extern FirebaseData fbdo;

#endif
