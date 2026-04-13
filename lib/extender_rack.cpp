#include <Arduino.h>

// =====================================================
// EXTENDER RACK TEST / DEMO
// TTGO T-Display S3 + H-bridge style motor driver
//
// Assumed from your wiring sketch:
// GPIO 1 -> ENA + ENB (shared PWM enable)
// GPIO 2 -> IN1 + IN3
// GPIO 3 -> IN2 + IN4
//
// This assumes both extender motors are meant to move together.
// If one motor spins the wrong way physically, reverse that motor's
// two output wires on the motor driver.
// =====================================================

// ---------- Pin mapping ----------
static const int EXT_PWM_PIN = 1;   // shared ENA + ENB
static const int EXT_IN1_PIN = 2;   // shared IN1 + IN3
static const int EXT_IN2_PIN = 3;   // shared IN2 + IN4

// ---------- PWM settings ----------
static const int EXT_PWM_CH = 4;
static const int PWM_FREQ   = 20000;
static const int PWM_RES    = 8;    // duty 0..255

// ---------- Direction tuning ----------
// If "extend" moves the rack the wrong way, change this to false.
static const bool EXTEND_IN1_HIGH = true;

// ---------- Speeds ----------
static const uint8_t MANUAL_SPEED_PERCENT     = 75;
static const uint8_t COLLECTION_SPEED_PERCENT = 85;
static const uint8_t HOPPER_SPEED_PERCENT     = 70;

// ---------- Timings to tune on real hardware ----------
static const uint32_t COLLECTION_EXTEND_MS = 1800;  // push scoop into dump zone
static const uint32_t RETAIN_RETRACT_MS    = 500;   // slight retract for rock retention
static const uint32_t HOPPER_EXTEND_MS     = 1200;  // extend near hopper
static const uint32_t FULL_RETRACT_MS      = 2200;  // retract to home

// =====================================================
// Helper functions
// =====================================================

uint8_t percentToDuty(uint8_t percent) {
  percent = constrain(percent, 0, 100);
  return (percent * 255) / 100;
}

void stopExtender() {
  ledcWrite(EXT_PWM_CH, 0);
  digitalWrite(EXT_IN1_PIN, LOW);
  digitalWrite(EXT_IN2_PIN, LOW);
  Serial.println("Extender STOP");
}

void driveExtender(uint8_t speedPercent, bool extend) {
  uint8_t duty = percentToDuty(speedPercent);

  bool in1State;
  bool in2State;

  if (extend) {
    in1State = EXTEND_IN1_HIGH;
    in2State = !EXTEND_IN1_HIGH;
    Serial.print("Extender EXTEND @ ");
  } else {
    in1State = !EXTEND_IN1_HIGH;
    in2State = EXTEND_IN1_HIGH;
    Serial.print("Extender RETRACT @ ");
  }

  Serial.print(speedPercent);
  Serial.println("%");

  digitalWrite(EXT_IN1_PIN, in1State);
  digitalWrite(EXT_IN2_PIN, in2State);
  ledcWrite(EXT_PWM_CH, duty);
}

void extendFor(uint8_t speedPercent, uint32_t runMs) {
  driveExtender(speedPercent, true);
  delay(runMs);
  stopExtender();
}

void retractFor(uint8_t speedPercent, uint32_t runMs) {
  driveExtender(speedPercent, false);
  delay(runMs);
  stopExtender();
}

// =====================================================
// Concept-aligned demo sequence
// =====================================================
// 1. Extend into dump zone
// 2. Pause at hard-coded end point
// 3. Retract slightly to help rock retention
// 4. Extend again near hopper
// 5. Retract fully after deposition
// =====================================================

void runExtenderConceptDemo() {
  Serial.println("\n--- STARTING EXTENDER CONCEPT DEMO ---");

  stopExtender();
  delay(1000);

  // Phase 1: extend for collection
  Serial.println("Phase 1: Extend into dump zone");
  extendFor(COLLECTION_SPEED_PERCENT, COLLECTION_EXTEND_MS);
  delay(1000);

  // Phase 2: slight retract for retention
  Serial.println("Phase 2: Slight retract for rock retention");
  retractFor(COLLECTION_SPEED_PERCENT, RETAIN_RETRACT_MS);
  delay(1000);

  // Phase 3: extend near hopper
  Serial.println("Phase 3: Extend near hopper");
  extendFor(HOPPER_SPEED_PERCENT, HOPPER_EXTEND_MS);
  delay(1500);

  // Phase 4: full retract after deposition
  Serial.println("Phase 4: Full retract to home");
  retractFor(HOPPER_SPEED_PERCENT, FULL_RETRACT_MS);
  delay(1000);

  stopExtender();
  Serial.println("--- EXTENDER CONCEPT DEMO COMPLETE ---\n");
}

// =====================================================
// Setup / Loop
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(EXT_IN1_PIN, OUTPUT);
  pinMode(EXT_IN2_PIN, OUTPUT);

  ledcSetup(EXT_PWM_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(EXT_PWM_PIN, EXT_PWM_CH);

  stopExtender();

  Serial.println("======================================");
  Serial.println("Extender rack control ready");
  Serial.println("Commands:");
  Serial.println("  e = extend");
  Serial.println("  r = retract");
  Serial.println("  s = stop");
  Serial.println("  d = run concept demo");
  Serial.println("======================================");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'e' || cmd == 'E') {
      driveExtender(MANUAL_SPEED_PERCENT, true);
    }
    else if (cmd == 'r' || cmd == 'R') {
      driveExtender(MANUAL_SPEED_PERCENT, false);
    }
    else if (cmd == 's' || cmd == 'S') {
      stopExtender();
    }
    else if (cmd == 'd' || cmd == 'D') {
      runExtenderConceptDemo();
    }

    // Clear any extra characters
    while (Serial.available()) {
      Serial.read();
    }
  }
}