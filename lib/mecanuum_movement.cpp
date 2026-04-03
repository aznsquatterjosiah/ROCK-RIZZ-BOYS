#include <Arduino.h>
#include <TFT_eSPI.h>

#define GLOBAL_PWM 1

#define FL_IN1 2
#define FL_IN2 3

#define FR_IN1 10
#define FR_IN2 11

#define BL_IN1 12
#define BL_IN2 13

#define BR_IN1 43
#define BR_IN2 44

#define PWM_CHANNEL 0
#define PWM_FREQ 20000
#define PWM_RESOLUTION 8   // 0-255 PWM

TFT_eSPI tft = TFT_eSPI();

int motorSpeed = 200;   // 0 to 255


// Basic motor helpers
void setGlobalSpeed(int speedValue)
{
  motorSpeed = constrain(speedValue, 0, 255);
  ledcWrite(PWM_CHANNEL, motorSpeed);
}

void stopMotor(int in1, int in2)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void driveMotorForward(int in1, int in2)
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void driveMotorReverse(int in1, int in2)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}


// Individual motors
void driveFL_Forward() { driveMotorForward(FL_IN1, FL_IN2); }
void driveFL_Reverse() { driveMotorReverse(FL_IN1, FL_IN2); }
void driveFL_Stop()    { stopMotor(FL_IN1, FL_IN2); }

void driveFR_Forward() { driveMotorForward(FR_IN1, FR_IN2); }
void driveFR_Reverse() { driveMotorReverse(FR_IN1, FR_IN2); }
void driveFR_Stop()    { stopMotor(FR_IN1, FR_IN2); }

void driveBL_Forward() { driveMotorForward(BL_IN1, BL_IN2); }
void driveBL_Reverse() { driveMotorReverse(BL_IN1, BL_IN2); }
void driveBL_Stop()    { stopMotor(BL_IN1, BL_IN2); }

void driveBR_Forward() { driveMotorForward(BR_IN1, BR_IN2); }
void driveBR_Reverse() { driveMotorReverse(BR_IN1, BR_IN2); }
void driveBR_Stop()    { stopMotor(BR_IN1, BR_IN2); }


// Robot movement
void robot_Stop()
{
  driveFL_Stop();
  driveFR_Stop();
  driveBL_Stop();
  driveBR_Stop();
}

void robot_Forward()
{
  driveFL_Forward();
  driveFR_Forward();
  driveBL_Forward();
  driveBR_Forward();
}

void robot_Reverse()
{
  driveFL_Reverse();
  driveFR_Reverse();
  driveBL_Reverse();
  driveBR_Reverse();
}

void robot_Right()
{
  // Move right
  driveFL_Forward();
  driveFR_Reverse();
  driveBL_Reverse();
  driveBR_Forward();
}

void robot_Left()
{
  // Move left
  driveFL_Reverse();
  driveFR_Forward();
  driveBL_Forward();
  driveBR_Reverse();
}

void robot_RotateCW()
{
  // clockwise rotation
  driveFL_Forward();
  driveFR_Reverse();
  driveBL_Forward();
  driveBR_Reverse();
}

void robot_RotateCCW()
{
  // anticlockwise rotation
  driveFL_Reverse();
  driveFR_Forward();
  driveBL_Reverse();
  driveBR_Forward();
}

// Optional diagonal motions
void robot_Diagonal_FrontRight()
{
  driveFL_Forward();
  driveFR_Stop();
  driveBL_Stop();
  driveBR_Forward();
}

void robot_Diagonal_FrontLeft()
{
  driveFL_Stop();
  driveFR_Forward();
  driveBL_Forward();
  driveBR_Stop();
}

void robot_Diagonal_BackRight()
{
  driveFL_Stop();
  driveFR_Reverse();
  driveBL_Reverse();
  driveBR_Stop();
}

void robot_Diagonal_BackLeft()
{
  driveFL_Reverse();
  driveFR_Stop();
  driveBL_Stop();
  driveBR_Reverse();
}

// Setup
void setup()
{
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);

  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);

  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);

  pinMode(BL_IN1, OUTPUT);
  pinMode(BL_IN2, OUTPUT);

  pinMode(BR_IN1, OUTPUT);
  pinMode(BR_IN2, OUTPUT);

  // ESP32 PWM setup
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(GLOBAL_PWM, PWM_CHANNEL);
  setGlobalSpeed(200);

  robot_Stop();

  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.println("Motor drive ready");
}


// Main loop
void loop()
{
  robot_Forward();
  delay(1000);

  robot_Stop();
  delay(500);

  robot_Reverse();
  delay(1000);

  robot_Stop();
  delay(500);

  robot_Right();
  delay(1000);

  robot_Stop();
  delay(500);

  robot_Left();
  delay(1000);

  robot_Stop();
  delay(500);

  robot_RotateCW();
  delay(1000);

  robot_Stop();
  delay(500);

  robot_RotateCCW();
  delay(1000);

  robot_Stop();
  delay(2000);
}