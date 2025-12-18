#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H
#pragma once
#include <Arduino.h>

struct AlertState {
    bool active;
    uint64_t startTime;
    String path;
};


// Inisialisasi alert
extern AlertState phAlert;
extern AlertState ecAlert;
extern AlertState tdsAlert;
extern AlertState tempAlert;

// Fungsi utama
void checkAlerts(float ph, float ec, float tds, float temp);

#endif
