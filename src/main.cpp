#include <Arduino.h>
#include <AccelStepper.h>

#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define SWITCH_PIN 4

uint8_t encoderState = 0;
uint8_t switchState = 0;
uint8_t timeSet = 0;

#define PEDAL_PIN 5
uint8_t pedalState = 0;

#define MOTOR_ENABLE_PIN 6
#define MOTOR_DIRECTION_PIN 7 
#define MOTOR_STEP_PIN 8
#define MOTOR_MAX_SPEED 2000
#define MOTOR_ACCELERATION 20000

AccelStepper stepper(1,MOTOR_STEP_PIN,MOTOR_DIRECTION_PIN);

void stepCW()
{
  if(timeSet < UINT8_MAX)
    timeSet++;
  Serial.print("Time set: ");
  Serial.print(timeSet);
  Serial.println("s");
}

void stepCCW()
{  
   if(timeSet > 0)
    timeSet--;
  Serial.print("Time set: ");
  Serial.print(timeSet);
  Serial.println("s");
}

void switchPress()
{
  Serial.println("- RESET -");
  timeSet = 0;
}

void pedalPress(){
  Serial.println("- PEDAL PRESS -");
  stepper.disableOutputs();
  stepper.setCurrentPosition(0);
  stepper.moveTo(MOTOR_MAX_SPEED * timeSet);
}

void readEncoder() {
  if(digitalRead(ENCODER_A_PIN) != digitalRead(ENCODER_B_PIN))
    stepCW();
  else
    stepCCW();  
}

void readSwitch(){
  uint8_t s = digitalRead(SWITCH_PIN);
  if(!switchState && s) switchPress();
  switchState = s;
}

void readPedal(){
  uint8_t p = digitalRead(PEDAL_PIN);
  if(!pedalState && p) pedalPress();
  pedalState = p;
}

void runMotor(){
  if(stepper.currentPosition() == MOTOR_MAX_SPEED * timeSet)
    stepper.enableOutputs();
  else
    stepper.run();
}

void setup()
{
  Serial.begin(115200);

  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), readEncoder, CHANGE);

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

void loop() {
  readSwitch();
  readPedal();
  runMotor();
}