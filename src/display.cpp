#include "display.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void initDisplay() {
  lcd.init();
  lcd.backlight();
}

void showStatus(float ph, float tds, float ec, float temp, bool isManual) {
  lcd.clear();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.setCursor(0, 3);
    lcd.print("Time Err");
    return;
  }

  // Baris 0
  lcd.setCursor(0, 0);
  lcd.printf("pH: %.1f TDS: %.0f", ph, tds);

  // Baris 1
  lcd.setCursor(0, 1);
  lcd.printf("EC: %.2f Temp: %.1fC", ec, temp);

  // Baris 2
  lcd.setCursor(0, 2);
  lcd.print("Mode: ");
  lcd.print(isManual ? "Manual" : "Auto");

  // Baris 3 (Tanggal + Waktu)
  lcd.setCursor(0, 3);
  lcd.printf("%02d/%02d/%04d %02d:%02d:%02d",
             timeinfo.tm_mday,
             timeinfo.tm_mon + 1,
             timeinfo.tm_year + 1900,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);
}
