#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESP32Servo.h>

TFT_eSPI tft = TFT_eSPI();

Servo wallLeftServo;
Servo wallRightServo;

// =====================================================
// TTGO backlight pin
// Uncomment if your board needs it
// =====================================================
#define TFT_BL 38

// =====================================================
// Pins
// =====================================================
#define US_TRIG 10
#define US_ECHO 11

#define WALL_LEFT_PIN  17
#define WALL_RIGHT_PIN 12

// =====================================================
// Ultrasonic settings
// =====================================================
const float STOP_DISTANCE_CM = 10.0f;
const float MIN_VALID_CM     = 2.0f;
const float MAX_VALID_CM     = 200.0f;
const unsigned long TIMEOUT_US = 30000;

// =====================================================
// Servo angles
// Change these if movement is opposite to what you want
// =====================================================
const int LEFT_START_ANGLE  = 0;
const int RIGHT_START_ANGLE = 180;

const int LEFT_FLIP_ANGLE   = 180;
const int RIGHT_FLIP_ANGLE  = 0;

// =====================================================
// Timing
// =====================================================
const unsigned long PICKUP_DELAY_MS = 1000;
const unsigned long SERVO_SETTLE_MS = 1500;

// =====================================================
// States
// =====================================================
enum RobotState {
  STATE_FIND_DROPZONE = 0,
  STATE_PICKUP,
  STATE_DONE
};

RobotState currentState = STATE_FIND_DROPZONE;

// =====================================================
// State tracking
// =====================================================
unsigned long stateStartTime = 0;
unsigned long lastScreenUpdate = 0;
bool servosFlipped = false;

// =====================================================
// Replace these with your real motor functions later
// =====================================================
void driveForwardSlow() {
  Serial.println("Driving forward...");
}

void stopDrive() {
  Serial.println("STOP");
}

// =====================================================
// Ultrasonic raw read
// =====================================================
float readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);

  if (duration == 0) {
    return -1.0f;
  }

  float distance = duration * 0.0343f / 2.0f;

  if (distance < MIN_VALID_CM || distance > MAX_VALID_CM) {
    return -1.0f;
  }

  return distance;
}

// =====================================================
// Stable ultrasonic read using median of valid samples
// This is better than simple averaging for noisy sensors
// =====================================================
float readStableDistanceCM() {
  float vals[5];
  int count = 0;

  for (int i = 0; i < 5; i++) {
    float d = readUltrasonicCM(US_TRIG, US_ECHO);
    if (d > 0) {
      vals[count++] = d;
    }
    delay(60);   // important: gives echo time to die out
  }

  if (count == 0) return -1.0f;

  // sort ascending
  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      if (vals[j] < vals[i]) {
        float temp = vals[i];
        vals[i] = vals[j];
        vals[j] = temp;
      }
    }
  }

  // median
  return vals[count / 2];
}

// =====================================================
// TFT screens
// =====================================================
void drawFindDropzoneScreen(float dist, bool moving) {
  if (millis() - lastScreenUpdate < 150) return;
  lastScreenUpdate = millis();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("State 1");

  tft.setCursor(10, 40);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Find dropzone");

  tft.setCursor(10, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print("Dist: ");
  if (dist < 0) {
    tft.println("No reading");
  } else {
    tft.print(dist, 1);
    tft.println(" cm");
  }

  tft.setCursor(10, 120);
  if (moving) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println("MOVING");
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("STOPPED");
  }
}

void drawPickupScreen(bool flipped) {
  if (millis() - lastScreenUpdate < 150) return;
  lastScreenUpdate = millis();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("State 2");

  tft.setCursor(10, 40);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Pickup");

  tft.setCursor(10, 80);
  if (!flipped) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.println("Waiting...");
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Servos flipped");
  }
}

void drawDoneScreen() {
  if (millis() - lastScreenUpdate < 250) return;
  lastScreenUpdate = millis();

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  tft.setCursor(10, 10);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("State 3");

  tft.setCursor(10, 40);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Done");

  tft.setCursor(10, 80);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Test complete");
}

// =====================================================
// State change helper
// =====================================================
void changeState(RobotState newState) {
  currentState = newState;
  stateStartTime = millis();
  lastScreenUpdate = 0;

  if (newState == STATE_PICKUP) {
    servosFlipped = false;
  }

  Serial.print("Changed to state: ");
  Serial.println((int)newState);
}

// =====================================================
// Setup
// =====================================================
void setup() {
  pinMode(15, OUTPUT); // as per your earlier request
  digitalWrite(15, HIGH);
  Serial.begin(115200);

  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
  digitalWrite(US_TRIG, LOW);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  wallLeftServo.setPeriodHertz(50);
  wallRightServo.setPeriodHertz(50);

  wallLeftServo.attach(WALL_LEFT_PIN, 500, 2400);
  wallRightServo.attach(WALL_RIGHT_PIN, 500, 2400);

  // initial servo positions
  wallLeftServo.write(LEFT_START_ANGLE);
  wallRightServo.write(RIGHT_START_ANGLE);

  changeState(STATE_FIND_DROPZONE);

  Serial.println("Robot state machine started");
}

// =====================================================
// Main loop
// =====================================================
void loop() {
  switch (currentState) {

    case STATE_FIND_DROPZONE: {
      // Only ultrasonic matters here
      float distanceCM = readStableDistanceCM();

      Serial.print("Distance: ");
      if (distanceCM < 0) {
        Serial.println("No reading");
        stopDrive();
        drawFindDropzoneScreen(distanceCM, false);
      }
      else if (distanceCM <= STOP_DISTANCE_CM) {
        Serial.print("Dropzone found at ");
        Serial.print(distanceCM, 1);
        Serial.println(" cm");

        stopDrive();
        drawFindDropzoneScreen(distanceCM, false);
        changeState(STATE_PICKUP);
      }
      else {
        Serial.print(distanceCM, 1);
        Serial.println(" cm");

        driveForwardSlow();
        drawFindDropzoneScreen(distanceCM, true);
      }

      break;
    }

    case STATE_PICKUP: {
      // Only servos matter here
      stopDrive();
      drawPickupScreen(servosFlipped);

      unsigned long elapsed = millis() - stateStartTime;

      if (!servosFlipped && elapsed >= PICKUP_DELAY_MS) {
        wallLeftServo.write(LEFT_FLIP_ANGLE);
        wallRightServo.write(RIGHT_FLIP_ANGLE);
        servosFlipped = true;

        Serial.println("Servos flipped");

        // restart timer so we can wait for servos to settle
        stateStartTime = millis();
      }

      if (servosFlipped && (millis() - stateStartTime >= SERVO_SETTLE_MS)) {
        changeState(STATE_DONE);
      }

      break;
    }

    case STATE_DONE: {
      // Ignore everything else in this state
      stopDrive();
      drawDoneScreen();
      break;
    }
  }

  delay(30);
}