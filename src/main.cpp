#include <Arduino.h>
#include <Encoder.h>
#include <AccelStepper.h>

#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define SWITCH_PIN 4

uint8_t currPos = 0;
uint8_t switchState = 0;
Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);

#define PEDAL_PIN 5
uint8_t pedalState = 0;

#define MOTOR_ENABLE_PIN 13
#define MOTOR_DIRECTION_PIN 11
#define MOTOR_STEP_PIN 12
#define MOTOR_MAX_SPEED 800
#define MOTOR_ACCELERATION 20000
uint8_t revs = 0;

AccelStepper stepper(1, MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);

void readEncoder()
{
  long newPos = encoder.read();
  if (newPos != currPos)
  {
    if (newPos <= 0)
    {
      currPos = 0;
      revs = 0;
      encoder.write(0);
    }
    else if (newPos % 2 == 0)
    {
      currPos = newPos;
      revs = currPos / 2;
    }
    Serial.print("Revs: ");
    Serial.println(revs);
  }
}

void readPedal()
{
  uint8_t p = digitalRead(PEDAL_PIN);
  if (!pedalState && p)
  {
    Serial.println("- PEDAL PRESS -");
    stepper.setCurrentPosition(0);
    stepper.moveTo(200 * revs);
  }
  pedalState = p;
}

void readSwitch()
{
  uint8_t s = digitalRead(SWITCH_PIN);
  if (!switchState && s)
  {
    Serial.println("- RESET -");
    encoder.write(0);
    currPos = 0;
    revs = 0;
  }
  switchState = s;
}

void runMotor()
{
  stepper.run();
}

void setup()
{
  Serial.begin(115200);

  pinMode(SWITCH_PIN, INPUT);
  pinMode(PEDAL_PIN, INPUT);

  pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_STEP_PIN, OUTPUT);

  stepper.setEnablePin(MOTOR_ENABLE_PIN);
  stepper.setMaxSpeed(MOTOR_MAX_SPEED);
  stepper.setAcceleration(MOTOR_ACCELERATION);
  stepper.enableOutputs();
}

void loop()
{
  readEncoder();
  readPedal();
  readSwitch();

  runMotor();
}