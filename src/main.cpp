#include <Arduino.h>
#include <TFT_eSPI.h>

//Forward Left
#define FL_PWM 1
#define FL_IN1 2
#define FL_IN2 3

//Forward Right
#define FR_PWM 43
#define FR_IN1 44
#define FR_IN2 18

//Back Left
#define BL_PWM 11
#define BL_IN1 12
#define BL_IN2 13

//Back Right
#define BR_PWM 17
#define BR_IN1 21
#define BR_IN2 16

TFT_eSPI tft = TFT_eSPI();

//Forward drive motors
void driveFL_Forward()
{
  digitalWrite(FL_IN1, HIGH);
  digitalWrite(FL_IN2, LOW);

  ledcWrite(0, 200); 
}

void driveFR_Forward()
{
  digitalWrite(FR_IN1, HIGH);
  digitalWrite(FR_IN2, LOW);

  ledcWrite(0, 200); 
}

void driveBL_Forward()
{
  digitalWrite(BL_IN1, HIGH);
  digitalWrite(BL_IN2, LOW);

  ledcWrite(0, 200); 
}

void driveBR_Forward()
{
  digitalWrite(BR_IN1, HIGH);
  digitalWrite(BR_IN2, LOW);

  ledcWrite(0, 200); 
}

//Reverse drive motors
void driveFL_Reverse()
{
  digitalWrite(FL_IN1, LOW);
  digitalWrite(FL_IN2, HIGH);

  ledcWrite(0, 200); 
}

void driveFR_Reverse()
{
  digitalWrite(FR_IN1, LOW);
  digitalWrite(FR_IN2, HIGH);

  ledcWrite(0, 200); 
}

void driveBL_Reverse()
{
  digitalWrite(BL_IN1, LOW);
  digitalWrite(BL_IN2, HIGH);

  ledcWrite(0, 200); 
}

void driveBR_Reverse()
{
  digitalWrite(BR_IN1, LOW);
  digitalWrite(BR_IN2, HIGH);

  ledcWrite(0, 200); 
}

//Robot drive forward
void robot_Forward()
{
  driveFL_Forward();
  driveFR_Forward();
  driveBL_Forward();
  driveBR_Forward();
}

//Robot drive reverse
void robot_Reverse()
{
  driveFL_Reverse();
  driveFR_Reverse();
  driveBL_Reverse();
  driveBR_Reverse();
}

//Robot strafe right
void robot_Right()
{
  driveFL_Forward();
  driveFR_Reverse();
  driveBL_Reverse();
  driveBR_Forward();
}

//Robot strafe left
void robot_Left()
{
  driveFL_Reverse();
  driveFR_Forward();
  driveBL_Forward();
  driveBR_Reverse();
}

//Setup
void setup()
{
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);

  pinMode(GLOBAL_PWM, OUTPUT);

  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);

  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);

  pinMode(BL_IN1, OUTPUT);
  pinMode(BL_IN2, OUTPUT);

  pinMode(BR_IN1, OUTPUT);
  pinMode(BR_IN2, OUTPUT);

  tft.setCursor(10, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4);
  tft.println("Motor driving ready.");
}

//Main
void loop() 
{
  // put your main code here, to run repeatedly:
}
