#include "alert_manager.h"
#include "firebase_utils.h"
#include <LiquidCrystal_I2C.h>
#include "time.h"

extern LiquidCrystal_I2C lcd;
// STATE ALERT
AlertState phAlert = {false, 0, 0};
AlertState ecAlert = {false, 0, 0};
AlertState tdsAlert = {false, 0, 0};
AlertState tempAlert = {false, 0, 0};

struct tm timeinfo;
// Fungsi helper
uint64_t nowMs() {
    if (!getLocalTime(&timeinfo)) {
        lcd.clear();
        lcd.print("Time Sync Err");
        return 0;
    }
    // Dapatkan unix timestamp ms dari struct tm
    return (uint64_t)mktime(&timeinfo) * 1000ULL;
}


bool outOfRange(float v, float minV, float maxV) {
    return v < minV || v > maxV;
}

bool beyondThreshold(float v, float last, float thr) {
    return abs(v - last) >= thr;
}

// Kirim alert ke Firebase pakai unix ms saja
void sendAlert(String type, float value, uint64_t startMs, uint64_t endMs) {
    FirebaseJson json;
    json.set("type", type);
    json.set("value", value);
    json.set("start_ms", (long long)startMs); // cast ke 64-bit
    json.set("end_ms", (long long)endMs);

    // Path pakai cast ke long long juga
    String path = "/devices/alert/" + String((long long)startMs);

    if (Firebase.setJSON(fbdo, path, json)) {
        Serial.printf("Alert sent: %s = %.2f\n", type.c_str(), value);
    } else {
        Serial.printf("Failed sending alert: %s\n", fbdo.errorReason().c_str());
    }
}

// Fungsi handle alert per sensor
void handleSingleAlert(float value, float minV, float maxV, float thr,
                       AlertState& alert, const char* type) 
{
    uint64_t currentMs = nowMs();
    if (currentMs == 0) return; // NTP belum ready, skip

    if (outOfRange(value, minV, maxV)) {
        if (!alert.active) {
            alert.active = true;
            alert.startTime = currentMs;

            alert.lastValue = value;
            sendAlert(type, value, (long long)alert.startTime, 0);
        } else if (beyondThreshold(value, (long long) alert.lastValue, thr)) {
            alert.lastValue = value;
            sendAlert(type, value, (long long)alert.startTime, 0);
        }
    } else {
        if (alert.active) {
            sendAlert(type, value, (long long) alert.startTime, currentMs);
            alert.active = false;
        }
    }
}


// Fungsi utama untuk semua sensor
void checkAlerts(float ph, float ec, float tds, float temp) {
    // Ambil threshold dari Firebase
    float ph_min = getFloat("/app/config/history_thresholds/ph_min");
    float ph_max = getFloat("/app/config/history_thresholds/ph_max");
    float ph_thr = getFloat("/app/config/history_thresholds/ph");

    float ec_min = getFloat("/app/config/history_thresholds/ec_min");
    float ec_max = getFloat("/app/config/history_thresholds/ec_max");
    float ec_thr = getFloat("/app/config/history_thresholds/ec_ms_cm");

    float tds_min = getFloat("/app/config/history_thresholds/tds_min");
    float tds_max = getFloat("/app/config/history_thresholds/tds_max");
    float tds_thr = getFloat("/app/config/history_thresholds/tds_ppm");

    float temp_min = getFloat("/app/config/history_thresholds/temp_min");
    float temp_max = getFloat("/app/config/history_thresholds/temp_max");
    float temp_thr = getFloat("/app/config/history_thresholds/temp_c");

    handleSingleAlert(ph, ph_min, ph_max, ph_thr, phAlert, "PH_OUT_OF_RANGE");
    handleSingleAlert(ec, ec_min, ec_max, ec_thr, ecAlert, "EC_OUT_OF_RANGE");
    handleSingleAlert(tds, tds_min, tds_max, tds_thr, tdsAlert, "TDS_OUT_OF_RANGE");
    handleSingleAlert(temp, temp_min, temp_max, temp_thr, tempAlert, "TEMP_OUT_OF_RANGE");
}
