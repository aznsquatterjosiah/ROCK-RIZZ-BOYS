#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// ---------- Ultrasonic pins ----------
#define US_TRIG 10
#define US_ECHO 11

// ---------- Stop distance ----------
const float STOP_DISTANCE_CM = 10.0;   // change to 5.0 to match report exactly
const float MIN_VALID_CM     = 2.0;
const float MAX_VALID_CM     = 200.0;
const unsigned long TIMEOUT_US = 30000;

// ---------- Read one ultrasonic value ----------
float readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);

  if (duration == 0) {
    return -1.0f; // no echo
  }

  float distance = duration * 0.0343f / 2.0f;

  if (distance < MIN_VALID_CM || distance > MAX_VALID_CM) {
    return -1.0f;
  }

  return distance;
}

// ---------- Simple average filter ----------
float readFilteredDistanceCM() {
  const int samples = 5;
  float sum = 0.0f;
  int count = 0;

  for (int i = 0; i < samples; i++) {
    float d = readUltrasonicCM(US_TRIG, US_ECHO);
    if (d > 0) {
      sum += d;
      count++;
    }
    delay(10);
  }

  if (count == 0) return -1.0f;
  return sum / count;
}

// ---------- Replace these with your real motor functions ----------
void driveForwardSlow() {
  // TODO: replace with your mecanum / motor driver code
  Serial.println("Driving forward...");
}

void stopDrive() {
  // TODO: replace with your real stop code
  Serial.println("STOP");
}

// ---------- Screen update ----------
void drawStatus(float dist, bool stopped) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.println("Collection Zone");

  tft.setCursor(10, 50);
  tft.print("Dist: ");
  if (dist < 0) {
    tft.println("No reading");
  } else {
    tft.print(dist, 1);
    tft.println(" cm");
  }

  tft.setCursor(10, 90);
  if (stopped) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("STOPPED");
  } else {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println("MOVING");
  }
}

// ---------- Main logic ----------
void setup() {
  Serial.begin(115200);

  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
  digitalWrite(US_TRIG, LOW);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  Serial.println("Single ultrasonic stop test");
}

void loop() {
  float distanceCM = readFilteredDistanceCM();

  Serial.print("Distance: ");
  if (distanceCM < 0) {
    Serial.println("No reading");
    stopDrive();                 // fail-safe
    drawStatus(distanceCM, true);
  } else if (distanceCM <= STOP_DISTANCE_CM) {
    Serial.println(String("At collection zone. Stop at <= ") + STOP_DISTANCE_CM + " cm");
    stopDrive();
    drawStatus(distanceCM, true);

    // stay stopped here
    while (true) {
      delay(100);
    }
  } else {
    Serial.println(String(distanceCM, 1) + " cm");
    driveForwardSlow();
    drawStatus(distanceCM, false);
  }

  delay(50);
}