#include <Arduino.h>

// ==========================================
// Extender rack motor test
// Forward -> Stop -> Backward -> Stop
// ==========================================

// Pins from your wiring diagram
const int EXT_EN  = 1;   // Enable / PWM
const int EXT_IN1 = 2;   // Direction 1
const int EXT_IN2 = 3;   // Direction 2

// PWM settings
const int PWM_CH   = 0;
const int PWM_FREQ = 20000;
const int PWM_RES  = 8;   // 0-255

// Speed (0 = off, 255 = full speed)
const int EXT_SPEED = 200;

void stopExtender() {
  digitalWrite(EXT_IN1, LOW);
  digitalWrite(EXT_IN2, LOW);
  ledcWrite(PWM_CH, 0);
  Serial.println("Extender stopped");
}

void extenderForward(int speedVal) {
  digitalWrite(EXT_IN1, HIGH);
  digitalWrite(EXT_IN2, LOW);
  ledcWrite(PWM_CH, speedVal);
  Serial.println("Extender forward");
}

void extenderBackward(int speedVal) {
  digitalWrite(EXT_IN1, LOW);
  digitalWrite(EXT_IN2, HIGH);
  ledcWrite(PWM_CH, speedVal);
  Serial.println("Extender backward");
}

void setup() {
  Serial.begin(115200);

  pinMode(EXT_IN1, OUTPUT);
  pinMode(EXT_IN2, OUTPUT);

  ledcSetup(PWM_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(EXT_EN, PWM_CH);

  stopExtender();
}

void loop() {
  extenderForward(EXT_SPEED);
  delay(2000);

  stopExtender();
  delay(1000);

  extenderBackward(EXT_SPEED);
  delay(2000);

  stopExtender();
  delay(1000);
}