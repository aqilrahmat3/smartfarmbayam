#include <unity.h>
#include "mock_utils.h"

// =====================
// ARDUINO vs NATIVE
// =====================
#ifndef ARDUINO
  #define HIGH 1
  #define LOW  0
  #define OUTPUT 1

  static int pin_state[10];
  static unsigned long fakeMillis = 0;

  void pinMode(int, int) {}
  void digitalWrite(int pin, int val) { pin_state[pin] = val; }
  int  digitalRead(int pin) { return pin_state[pin]; }
  unsigned long millis() { return fakeMillis += 1000; }
  void delay(int) {}
#endif

// =====================
// PIN DEFINE
// =====================
#define RELAY_PUMP_ACID   2
#define RELAY_PUMP_NUTRI  4

// =====================
// LOGIC UTAMA
// =====================
#include "auto_mode.h"

// =====================
// SETUP & TEARDOWN
// =====================
void setUp(void) {
    pinMode(RELAY_PUMP_ACID, OUTPUT);
    pinMode(RELAY_PUMP_NUTRI, OUTPUT);

    digitalWrite(RELAY_PUMP_ACID, LOW);
    digitalWrite(RELAY_PUMP_NUTRI, LOW);

    firebase_bool_sent = false;
    lastAcidRun = 0;
    lastNutrientRun = 0;
}

void tearDown(void) {}

// =====================
// 🧪 TEST PH
// =====================
void test_ph_tinggi_harus_nyalakan_pompa_acid(void) {
    mock_ph_value = 8.0;

    lastAcidRun = millis() - COOLDOWN_TIME - 1;

    handleAutoMode();

    TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_PUMP_ACID));
}


void test_ph_rendah_harus_matikan_pompa_acid(void) {
    mock_ph_value = 4.0;

    digitalWrite(RELAY_PUMP_ACID, HIGH);
    handleAutoMode();

    TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_PUMP_ACID));
}

// =====================
// 🧪 TEST TDS
// =====================
void test_tds_rendah_harus_nyalakan_pompa_nutrisi(void) {
    mock_tds_value = 500;
    mock_ec_value  = 1.0;

    lastNutrientRun = millis() - COOLDOWN_TIME - 1;

    handleAutoMode();

    TEST_ASSERT_EQUAL(HIGH, digitalRead(RELAY_PUMP_NUTRI));
}


void test_tds_tinggi_harus_matikan_pompa_nutrisi(void) {
    mock_tds_value = 2000;

    digitalWrite(RELAY_PUMP_NUTRI, HIGH);
    handleAutoMode();

    TEST_ASSERT_EQUAL(LOW, digitalRead(RELAY_PUMP_NUTRI));
}

// =====================
// UNITY RUNNER
// =====================
#ifdef ARDUINO
void setup() {
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_ph_tinggi_harus_nyalakan_pompa_acid);
    RUN_TEST(test_ph_rendah_harus_matikan_pompa_acid);
    RUN_TEST(test_tds_rendah_harus_nyalakan_pompa_nutrisi);
    RUN_TEST(test_tds_tinggi_harus_matikan_pompa_nutrisi);

    UNITY_END();
}
void loop() {}
#else
int main() {
    UNITY_BEGIN();

    RUN_TEST(test_ph_tinggi_harus_nyalakan_pompa_acid);
    RUN_TEST(test_ph_rendah_harus_matikan_pompa_acid);
    RUN_TEST(test_tds_rendah_harus_nyalakan_pompa_nutrisi);
    RUN_TEST(test_tds_tinggi_harus_matikan_pompa_nutrisi);

    return UNITY_END();
}
#endif
