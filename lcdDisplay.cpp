#include "Arduino.h"
#include "lcdDisplay.h"
#include <LiquidCrystal.h>

//can add global vars
int prevMode = 1;
int prevMoisture = 0;
int prevThresh = 0;
int prevLastWater = 0;

// initialize the LCD interface pins
const int rs = 9, en = 8, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal LCD(rs, en, d4, d5, d6, d7);

dclass::dclass() {
}

void dclass::SETUP() {
  LCD.begin(16, 2); // set LCD's columns and rows
  LCD.print("Booting Up...");
}


void dclass::updateLCD(int mode, int moisture, int thresh, unsigned long lastWater) {
  //convert lastWater to string
  //ms to seconds

  char lastWaterDisplay[30];
  if (lastWater < 60) { //display seconds
    snprintf(lastWaterDisplay, 30, "%ds", lastWater);

  } else if (lastWater < 3600) { //display minutes
    lastWater = lastWater / 60;
    snprintf(lastWaterDisplay, 30, "%dmin", lastWater);

  } else if (lastWater < 86400) { //display hours
    float decimalNumber = (((float)lastWater / 60.0) / 60.0);
    char floatString[10];
    dtostrf(decimalNumber, 3, 1, floatString);
    sprintf(lastWaterDisplay, "%shrs", floatString);

  } else { //display days
    float decimalNumber = ((((float)lastWater / 60.0) / 60.0) / 24.0);
    char floatString[20];
    dtostrf(decimalNumber, 3, 1, floatString);
    sprintf(lastWaterDisplay, "%sdays", floatString);
  }

  if (prevMode != mode || prevMoisture != moisture || prevThresh != thresh || prevLastWater != lastWater) {
    // normal screen
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(moisture);
    LCD.print("% ");
    LCD.print("thresh: ");
    LCD.print(thresh);
    LCD.print("%");
    LCD.setCursor(0, 1);
    LCD.print("wet ");
    LCD.print(lastWaterDisplay);
    LCD.print(" ago");
    prevMode = mode;
    prevMoisture = moisture;
    prevThresh = thresh;
    prevLastWater = lastWater;
  }

}

void dclass::waterLCD(int mode) {
  if (prevMode != mode) {
    // Watering... (show how many cups it is going to water?)
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Watering...");
    prevMode = mode;
  }

}

void dclass::emptyTankLCD(int mode, int moisture) {
  if (prevMode != mode || prevMoisture != moisture) {
    // Empty Tank
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(moisture);
    LCD.print("% ");
    LCD.setCursor(0, 1);
    LCD.print("Empty Tank!!!");
    prevMode = mode;
    prevMoisture = moisture;
  }
}

void dclass::justPollLCD(int mode, int moisture) {
  if (prevMode != mode || prevMoisture != moisture) {
    // normal screen
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(moisture);
    LCD.print("% ");
    prevMode = mode;
    prevMoisture = moisture;
  }
}

void dclass::stopLCD(int mode) {
  if (prevMode != mode) {
    // Empty Tank
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Program Paused..");
    prevMode = mode;
  }
}

dclass lcd = dclass();
