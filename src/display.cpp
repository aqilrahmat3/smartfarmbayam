#include "display.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void initDisplay() {
  lcd.init();
  lcd.backlight();
}

void showStatus(float ph, float tds, float ec, float temp, String lightCat, bool isManual) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("pH: %.1f TDS: %.0f", ph, tds);
  lcd.setCursor(0,1);
  lcd.printf("EC: %.2f Temp: %.1fC", ec, temp);
  lcd.setCursor(0,2);
  lcd.print("Light: "); lcd.print(lightCat);
  lcd.setCursor(0,3);
  lcd.print("Mode: "); lcd.print(isManual ? "Manual" : "Auto");
}
