#include "alert_manager.h"
#include "firebase_utils.h"

// STATE ALERT
AlertState phAlert = {false, 0, 0};
AlertState ecAlert = {false, 0, 0};
AlertState tdsAlert = {false, 0, 0};
AlertState tempAlert = {false, 0, 0};

// Fungsi helper
unsigned long nowMs() {
    return millis();
}

bool outOfRange(float v, float minV, float maxV) {
    return v < minV || v > maxV;
}

bool beyondThreshold(float v, float last, float thr) {
    return abs(v - last) >= thr;
}

// Kirim alert ke Firebase
void sendAlert(String type, float value, unsigned long startMs, unsigned long endMs) {
    struct tm timeinfo;
    char startISO[30] = "";
    char endISO[30] = "";

    // Konversi startTime ke ISO jika NTP ready
    if (getLocalTime(&timeinfo)) {
        time_t t = time(nullptr) - (millis() - startMs)/1000; // perkiraan start time
        gmtime_r(&t, &timeinfo);
        sprintf(startISO, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec);
    }

    // Konversi endTime jika >0
    if (endMs > 0 && getLocalTime(&timeinfo)) {
        time_t t = time(nullptr) - (millis() - endMs)/1000;
        gmtime_r(&t, &timeinfo);
        sprintf(endISO, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec);
    }

    FirebaseJson json;
    json.set("type", type);
    json.set("value", value);
    json.set("start_ms", startMs);
    json.set("end_ms", endMs);
    json.set("start_time", startISO);
    json.set("end_time", endISO);

    String path = "/devices/alert/" + String(startMs);

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
    if (outOfRange(value, minV, maxV)) {
        // sensor keluar range
        if (!alert.active) {
            alert.active = true;
            alert.startTime = nowMs();
            alert.lastValue = value;
            sendAlert(type, value, alert.startTime, 0); // kirim alert pertama
        }
        else if (beyondThreshold(value, alert.lastValue, thr)) {
            // perubahan signifikan
            alert.lastValue = value;
            sendAlert(type, value, alert.startTime, 0);
        }
    } else {
        // sensor kembali normal
        if (alert.active) {
            sendAlert(type, value, alert.startTime, nowMs());
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
