#include "alert_manager.h"
#include "firebase_utils.h"
#include <LiquidCrystal_I2C.h>
#include "time.h"

extern LiquidCrystal_I2C lcd;
// STATE ALERT
AlertState phAlert   = {false, 0, ""};
AlertState ecAlert   = {false, 0, ""};
AlertState tdsAlert  = {false, 0, ""};
AlertState tempAlert = {false, 0, ""};

struct tm timeinfo;
// Fungsi helper
uint64_t getNowMs() {
    static bool synced = false;
    static uint64_t baseUnixMs = 0;
    static unsigned long baseMillis = 0;

    if (!synced) {
        struct tm t;
        if (getLocalTime(&t)) {
            baseUnixMs = (uint64_t)mktime(&t) * 1000ULL;
            baseMillis = millis();
            synced = true;
        } else {
            return 0;
        }
    }
    return baseUnixMs + (millis() - baseMillis);
}



bool outOfRange(float v, float minV, float maxV) {
    return v < minV || v > maxV;
}

bool beyondThreshold(float v, float last, float thr) {
    return abs(v - last) >= thr;
}

// Kirim alert ke Firebase pakai unix ms saja
void createAlert(AlertState& alert, const char* type, float value, uint64_t now) {
    FirebaseJson json;
    json.set("type", type);
    json.set("start_ms", (long long)now);
    json.set("value", value);
    json.set("end_ms", 0);

    String path = "/devices/alert/" + String((long long)now);

    if (Firebase.setJSON(fbdo, path, json)) {
        alert.path = path;
        Serial.println("Alert START");
    }
}

void endAlert(AlertState& alert, float value, uint64_t now) {
    FirebaseJson json;
    json.set("end_ms", (long long)now);
    json.set("value", value);

    Firebase.updateNode(fbdo, alert.path, json);
    Serial.println("Alert END");
}
void handleSingleAlert(
    float value, float minV, float maxV,
    AlertState& alert, const char* type, uint64_t now
) {
    if (value < minV || value > maxV) {
        if (!alert.active) {
            alert.active = true;
            alert.startTime = now;
            createAlert(alert, type, value, now);
        }
    } else {
        if (alert.active) {
            endAlert(alert, value, now);
            alert.active = false;
            alert.path = "";
        }
    }
}

struct Thresholds {
    float ph_min, ph_max;
    float ec_min, ec_max;
    float tds_min, tds_max;
    float temp_min, temp_max;
} th;

unsigned long lastFetch = 0;

void fetchThresholds() {
    if (millis() - lastFetch < 5000) return;

    th.ph_min   = getFloat("/app/config/thresholds/ph_min");
    th.ph_max   = getFloat("/app/config/thresholds/ph_max");
    th.ec_min   = getFloat("/app/config/thresholds/ec_min");
    th.ec_max   = getFloat("/app/config/thresholds/ec_max");
    th.tds_min  = getFloat("/app/config/thresholds/tds_min");
    th.tds_max  = getFloat("/app/config/thresholds/tds_max");
    th.temp_min = getFloat("/app/config/thresholds/temp_min_c");
    th.temp_max = getFloat("/app/config/thresholds/temp_max_c");

    lastFetch = millis();
}
void checkAlerts(float ph, float ec, float tds, float temp) {
    fetchThresholds();

    uint64_t now = getNowMs();
    if (now == 0) return;

    handleSingleAlert(ph,   th.ph_min,   th.ph_max,   phAlert,   "PH_OUT_OF_RANGE",   now);
    handleSingleAlert(ec,   th.ec_min,   th.ec_max,   ecAlert,   "EC_OUT_OF_RANGE",   now);
    handleSingleAlert(tds,  th.tds_min,  th.tds_max,  tdsAlert,  "TDS_OUT_OF_RANGE",  now);
    handleSingleAlert(temp, th.temp_min, th.temp_max, tempAlert, "TEMP_OUT_OF_RANGE", now);
}
