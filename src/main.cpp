/*****************************************************************************/
/*!
  @file main.cpp

  @mainpage The SkeinTwister Arduino project

  @section intro Introduction

  PlatformIO project for SkeinTwister

  @section author Author

  Written by Herr Lukas for the SkeinTwister Arduino project.

  @section license License

  MIT License, check license.txt for more information
  All text above, and the splash screen below must be included in any
  redistribution
*/
/*****************************************************************************/

/* Library includes */
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Encoder.h>
#include <AccelStepper.h>

/* Rotary encoder definition */
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define MIDDLE_PIN 4
Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);

int32_t currPos = 0;
int switchState = 0;

/* Pedal definition */
#define PEDAL_PIN 10
int pedalState = 0;

/* Stepper motor definition */
#define MOTOR_ENABLE_PIN 13
#define MOTOR_DIRECTION_PIN 11
#define MOTOR_STEP_PIN 12
AccelStepper stepper(1, MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);

int speed = 800;
int acceleration = 20000;
int revs = 0;
bool calibration = 0;

/* Display definition */
#define SCLK_PIN 5
#define DIN_PIN 6
#define DC_PIN 7
#define CS_PIN 9
#define RST_PIN 8
Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK_PIN, DIN_PIN, DC_PIN, CS_PIN, RST_PIN);

int selectedMenuItem = 1;
int selectedFrame = 1;
int selectedPage = 1;
int lastMenuItem = 1;
int lastStepperState = 1;

boolean up = false;
boolean down = false;
boolean middle = false;

String menuItem1 = "Revolutions";
String menuItem2 = "Speed";
String menuItem3 = "Acceleration";
String menuItem4 = "Calibrate";

void readEncoder()
{
  int32_t newPos = encoder.read();
  if (newPos / 2 > currPos)
  {
    currPos = newPos / 2;
    down = true;
  }
  else if (newPos / 2 < currPos)
  {
    currPos = newPos / 2;
    up = true;
  }

  int s = digitalRead(MIDDLE_PIN);
  if (!switchState && s)
  {
    middle = true;
  }
  switchState = s;
}

void readPedal()
{
  int p = digitalRead(PEDAL_PIN);
  if (!pedalState && p)
  {
    stepper.setCurrentPosition(0);
    stepper.moveTo(200 * revs);
  }
  pedalState = p;
}

void processInput()
{
  if (up && selectedPage == 1)
  {

    up = false;
    if (selectedMenuItem == 2 && selectedFrame == 2)
    {
      selectedFrame--;
    }
    lastMenuItem = selectedMenuItem;
    selectedMenuItem--;
    if (selectedMenuItem == 0)
    {
      selectedMenuItem = 1;
    }
  }

  if (up && selectedPage == 2)
  {
    up = false;
    if (selectedMenuItem == 1)
    {
      revs--;
    }
    else if (selectedMenuItem == 2)
    {
      speed--;
      stepper.setMaxSpeed(speed);
    }
    else if (selectedMenuItem == 3)
    {
      acceleration--;
      stepper.setAcceleration(acceleration);
    }
    else if (selectedMenuItem == 4)
    {
      calibration = !calibration;
      if (calibration)
      {
        stepper.disableOutputs();
      }
      else
      {
        stepper.enableOutputs();
      }
    }
  }

  if (down && selectedPage == 1)
  {

    down = false;
    if (selectedMenuItem == 3 && lastMenuItem == 2)
    {
      selectedFrame++;
    }
    lastMenuItem = selectedMenuItem;
    selectedMenuItem++;
    if (selectedMenuItem == 5)
    {
      selectedMenuItem--;
    }
  }

  if (down && selectedPage == 2)
  {
    down = false;
    if (selectedMenuItem == 1)
    {
      revs++;
    }
    else if (selectedMenuItem == 2)
    {
      speed++;
      stepper.setMaxSpeed(speed);
    }
    else if (selectedMenuItem == 3)
    {
      acceleration++;
      stepper.setAcceleration(acceleration);
    }
    else if (selectedMenuItem == 4)
    {
      calibration = !calibration;
      if (calibration)
      {
        stepper.disableOutputs();
      }
      else
      {
        stepper.enableOutputs();
      }
    }
  }

  if (middle)
  {
    middle = false;
    if (selectedPage == 1)
    {
      selectedPage = 2;
    }
    else
    {
      selectedPage = 1;
    }
  }
}

void drawMenuItem(String item, int position, boolean selected)
{
  if (selected)
  {
    display.setTextColor(WHITE, BLACK);
  }
  else
  {
    display.setTextColor(BLACK, WHITE);
  }
  display.setCursor(0, position);
  display.print(">" + item);
}

void drawIntMenuItem(String menuItem, int value)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print(menuItem);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Value");
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(value);
  display.setTextSize(2);
  display.display();
}

void drawBoolMenuItem(String menuItem, bool value,
                      String trueValue, String falseValue)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(15, 0);
  display.print(menuItem);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Value");
  display.setTextSize(2);
  display.setCursor(5, 25);
  if (value)
  {
    display.print(trueValue);
  }
  else
  {
    display.print(falseValue);
  }
  display.setTextSize(2);
  display.display();
}

void drawMenu()
{
  if (selectedPage == 1)
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("MAIN MENU");
    display.drawFastHLine(0, 10, 83, BLACK);

    if (selectedMenuItem == 1 && selectedFrame == 1)
    {
      drawMenuItem(menuItem1, 15, true);
      drawMenuItem(menuItem2, 25, false);
      drawMenuItem(menuItem3, 35, false);
    }
    else if (selectedMenuItem == 2 && selectedFrame == 1)
    {
      drawMenuItem(menuItem1, 15, false);
      drawMenuItem(menuItem2, 25, true);
      drawMenuItem(menuItem3, 35, false);
    }
    else if (selectedMenuItem == 3 && selectedFrame == 1)
    {
      drawMenuItem(menuItem1, 15, false);
      drawMenuItem(menuItem2, 25, false);
      drawMenuItem(menuItem3, 35, true);
    }
    else if (selectedMenuItem == 4 && selectedFrame == 2)
    {
      drawMenuItem(menuItem2, 15, false);
      drawMenuItem(menuItem3, 25, false);
      drawMenuItem(menuItem4, 35, true);
    }

    else if (selectedMenuItem == 3 && selectedFrame == 2)
    {
      drawMenuItem(menuItem2, 15, false);
      drawMenuItem(menuItem3, 25, true);
      drawMenuItem(menuItem4, 35, false);
    }
    else if (selectedMenuItem == 2 && selectedFrame == 2)
    {
      drawMenuItem(menuItem2, 15, true);
      drawMenuItem(menuItem3, 25, false);
      drawMenuItem(menuItem4, 35, false);
    }
    else if (selectedMenuItem == 2 && selectedFrame == 2)
    {
      drawMenuItem(menuItem2, 15, true);
      drawMenuItem(menuItem3, 25, false);
      drawMenuItem(menuItem4, 35, false);
    }

    display.display();
  }
  else if (selectedPage == 2)
  {
    if (selectedMenuItem == 1)
    {
      drawIntMenuItem(menuItem1, revs);
    }
    if (selectedMenuItem == 2)
    {
      drawIntMenuItem(menuItem2, speed);
    }
    if (selectedMenuItem == 3)
    {
      drawIntMenuItem(menuItem3, acceleration);
    }
    if (selectedMenuItem == 4)
    {
      drawBoolMenuItem(menuItem4, calibration, "ON", "OFF");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(MIDDLE_PIN, INPUT);
  pinMode(PEDAL_PIN, INPUT);

  pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_STEP_PIN, OUTPUT);

  stepper.setEnablePin(MOTOR_ENABLE_PIN);
  stepper.setMaxSpeed(speed);
  stepper.setAcceleration(acceleration);
  stepper.enableOutputs();

  display.begin();
  display.clearDisplay();
  display.setContrast(50);
}

void loop()
{
  if (stepper.isRunning() == false)
  {
    readEncoder();
    readPedal();
  }

  if (down || up || middle)
  {
    processInput();
    drawMenu();
  }

  stepper.run();
}