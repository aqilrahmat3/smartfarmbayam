#include "firebase_utils.h"
#include <LiquidCrystal_I2C.h>
#include "time.h"

// ======== External Objects ========
extern LiquidCrystal_I2C lcd;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ======== Firebase Credentials ========
#define FIREBASE_HOST "smartfarmbayam-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_EMAIL "purnama@gmail.com"
#define FIREBASE_PASSWORD "pulsapulsa"
#define FIREBASE_API_KEY "AIzaSyCvMJlHBeVkXnbHEcPmkUGMuYekWH9VBdE"


// =========================================
// 🔧 INISIALISASI FIREBASE
// =========================================
void initFirebase() {

    config.host = FIREBASE_HOST;
    config.api_key = FIREBASE_API_KEY;

    auth.user.email = FIREBASE_EMAIL;
    auth.user.password = FIREBASE_PASSWORD;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // sinkron waktu menggunakan NTP
    configTime(7 * 3600, 0, "pool.ntp.org");
}



// =========================================
// 🔍 FUNGSI BACA BOOLEAN DARI FIREBASE
// =========================================
bool getBool(const String& path) {
    if (Firebase.getBool(fbdo, path.c_str())) {
        return fbdo.boolData();
    }
    return false;
}



// =========================================
// 🔍 FUNGSI BACA FLOAT DARI FIREBASE
// =========================================
float getFloat(const String& path) {
    if (Firebase.getFloat(fbdo, path.c_str())) {
        return fbdo.floatData();
    }
    return 0.0;
}



// =========================================
// 💾 UPDATE BOOLEAN KE FIREBASE (STATUS DEVICE)
// =========================================
void setBool(const String& path, bool value) {
    Firebase.setBool(fbdo, path.c_str(), value);
}



// =========================================
// 📡 KIRIM DATA SENSOR KE FIREBASE
// disimpan dalam logs history ESP32
// =========================================
void sendToFirebase(float ph, float tds, float ec, float temp) {

    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        lcd.clear();
        lcd.print("Time Sync Err");
        return;
    }

    // 64-bit timestamp
    uint64_t unix_ms = (uint64_t)mktime(&timeinfo) * 1000ULL;

    // Path pakai unix_ms 64-bit
    String path = "/devices/esp32_001/readings/" + String((long long)unix_ms);

    FirebaseJson json;
    json.set("ph", ph);
    json.set("tds_ppm", tds);
    json.set("ec_ms_cm", ec);
    json.set("temp_c", temp);
    json.set("unix_ms", (long long) unix_ms);

    Firebase.setJSON(fbdo, path.c_str(), json);
}

