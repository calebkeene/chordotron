// -*- mode: C++ -*-
// Make a single stepper bounce from one limit to another
//
#include <AccelStepper.h>

const int enableStepperA = 48; 
const int MS1StepperA = 49; 
const int MS2StepperA = 50;
const int MS3StepperA = 51;
const int stepStepperA = 52;
const int setDirStepperA = 53;

// Stepper motor driver B
const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepStepperB = 9;
const int setDirStepperB = 8;

AccelStepper stepper(1, stepStepperA, setDirStepperA);

void setup()
{  

  pinMode(MS1StepperA, OUTPUT);
  pinMode(MS2StepperA, OUTPUT);
  pinMode(MS3StepperA, OUTPUT);
  pinMode(enableStepperA, OUTPUT);
  pinMode(stepStepperA, OUTPUT);
  pinMode(setDirStepperA, OUTPUT);

  // microstep resolution
  digitalWrite(MS1StepperA, LOW);
  digitalWrite(MS2StepperA, LOW);
  digitalWrite(MS3StepperA, LOW);

  // set initial direction
  digitalWrite(setDirStepperA, HIGH);
  
  // enable (active low)
  digitalWrite(enableStepperA, LOW);

  Serial.begin(9600);

  stepper.setMaxSpeed(400); // 200 steps/sec = 1rev/sec = 60rpm
  stepper.setAcceleration(100);
  stepper.moveTo(200); // seems to behave as 100/turn for this
}
void loop()
{
    if (stepper.distanceToGo() == 0){
      // change direction once position is reached
      Serial.println("changing direction");
      stepper.moveTo(-stepper.currentPosition());
    }
    stepper.run();
}
