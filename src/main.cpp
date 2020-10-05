#include <Arduino.h>
#include <Encoder.h>
#include <AccelStepper.h>

#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define SWITCH_PIN 4

long oldRevs = -999;
uint8_t currRevs = 0;
uint8_t switchState = 0;
Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);

#define PEDAL_PIN 5
uint8_t pedalState = 0;

#define MOTOR_ENABLE_PIN 6
#define MOTOR_DIRECTION_PIN 7
#define MOTOR_STEP_PIN 8
#define MOTOR_MAX_SPEED 800
#define MOTOR_ACCELERATION 20000

AccelStepper stepper(1, MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);

void readEncoder()
{
  long newRevs = encoder.read();
  if (newRevs != currRevs)
  {
    if (newRevs >= 0)
    {
      currRevs = newRevs;
    }
    else
    {
      encoder.write(0);
    }
    Serial.print("Revs: ");
    Serial.println(currRevs);
  }
}

void readPedal()
{
  uint8_t p = digitalRead(PEDAL_PIN);
  if (!pedalState && p)
  {
    Serial.println("- PEDAL PRESS -");
    stepper.setCurrentPosition(0);
    stepper.moveTo(200 * currRevs);
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
    currRevs = 0;
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