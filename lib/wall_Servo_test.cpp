#include <Arduino.h>
#include <ESP32Servo.h>

Servo wallLeftServo;
Servo wallRightServo;

static const int WALL_LEFT_PIN  = 17;
static const int WALL_RIGHT_PIN = 12;

void setup() {
  Serial.begin(115200);

  wallLeftServo.setPeriodHertz(50);
  wallRightServo.setPeriodHertz(50);

  wallLeftServo.attach(WALL_LEFT_PIN, 500, 2400);
  wallRightServo.attach(WALL_RIGHT_PIN, 500, 2400);
}

void loop() {
  wallLeftServo.write(0);
  wallRightServo.write(180);
  delay(2000);

  wallLeftServo.write(180);
  wallRightServo.write(0);
  delay(2000);
}