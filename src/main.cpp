#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {

  pinMode(15, OUTPUT); // as per your earlier request
  digitalWrite(15, HIGH);
  
  // Turn on backlight
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  // Start display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Print "hi" in the middle
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("hi", tft.width() / 2, tft.height() / 2, 4);
}

void loop() {
}