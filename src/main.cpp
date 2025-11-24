#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"

// ===== WiFi =====
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ===== Firebase =====
// Gunakan user email & password Firebase Authentication
#define FIREBASE_HOST "smartfarmbayam-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_EMAIL "purnama@gmail.com"
#define FIREBASE_PASSWORD "purnama"
#define FIREBASE_API_KEY "AIzaSyCvMJlHBeVkXnbHEcPmkUGMuYekWH9VBdE"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ===== Sensor =====
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define PH_PIN 33
#define TDS_PIN 32
#define EC_PIN 35
#define LDR_AO   34
#define LDR_DO   18

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ===== NTP =====
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7*3600; // WIB
const int daylightOffset_sec = 0;

// ===== Fungsi kategori cahaya =====
String getLightCategory(int value) {
  if(value<800) return "Gelap";
  else if(value<1600) return "Redup";
  else if(value<2600) return "Normal";
  else if(value<3500) return "Terang";
  else return "SgtTerang";
}

// ===== Fungsi kirim data ke Firebase =====
void sendToFirebase(float ph, float tds, float ec, float temp, String lightCat) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    lcd.print("Gagal mendapatkan waktu NTP");
    return;
  }

  unsigned long unix_ms = mktime(&timeinfo)*1000UL;

  char isoTime[30];
  sprintf(isoTime, "%04d-%02d-%02dT%02d:%02d:%02dZ",
          timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  String path = "/devices/esp32_001/readings/"+ String(unix_ms); 

  FirebaseJson json;
  json.set("ph", ph);
  json.set("tds_ppm", tds);
  json.set("ec_ms_cm", ec);
  json.set("temp_c", temp);
  json.set("light_category", lightCat);
  json.set("timestamp_iso", isoTime);
  json.set("unix_ms", unix_ms);

  if (Firebase.setJSON(fbdo, path.c_str(), json)) {
      Serial.print("Data sent to Firebase successfully");
  } else {
      Serial.print("Firebase send failed: ");
      Serial.print(fbdo.errorReason());
  }

}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LDR_DO, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("System Starting...");
  delay(1500);
  lcd.clear();

  // ===== WiFi =====
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  lcd.setCursor(0,1);
  lcd.print("WiFi connected");

  // ===== Firebase =====
  config.host = FIREBASE_HOST;
  config.api_key = FIREBASE_API_KEY;
  auth.user.email = FIREBASE_EMAIL;
  auth.user.password = FIREBASE_PASSWORD;

  // Inisialisasi Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Tunggu sampai token siap
  unsigned long startTime = millis();
  bool loginOK = false;

  // ===== NTP =====
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {
  // Baca sensor / simulasi
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int rawPh = analogRead(PH_PIN);
  float ph = (rawPh / 4095.0) * 14.0;

  int rawTDS = analogRead(TDS_PIN);     
  float tds = map(rawTDS, 0, 4095, 0, 2000); 

  int rawEC = analogRead(EC_PIN);       
  float ec = map(rawEC, 0, 4095, 0, 3);      

  int lightAnalog = analogRead(LDR_AO);
  String lightCat = getLightCategory(lightAnalog);

  // Kirim ke Firebase
  sendToFirebase(ph, tds, ec, temp, lightCat);

  // // ===== LCD =====
   lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("pH: %.1f TDS: %.0f", ph, tds);
  lcd.setCursor(0,1);
  lcd.printf("EC: %.2f Temp: %.1fC", ec, temp);
  lcd.setCursor(0,2);
  lcd.print("Light: "); lcd.print(lightCat);


  delay(2000);
}
