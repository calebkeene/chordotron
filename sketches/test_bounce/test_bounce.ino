// -*- mode: C++ -*-
// Make a single stepper bounce from one limit to another
//
#include <AccelStepper.h>

// Stepper motor driver B
const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepStepperB = 9;
const int setDirStepperB = 8;

AccelStepper stepper(1, stepStepperB, setDirStepperB);

void setup()
{  

  pinMode(MS1StepperB, OUTPUT);
  pinMode(MS2StepperB, OUTPUT);
  pinMode(MS3StepperB, OUTPUT);
  pinMode(enableStepperB, OUTPUT);
  pinMode(stepStepperB, OUTPUT);
  pinMode(setDirStepperB, OUTPUT);

  // microstep resolution
  digitalWrite(MS1StepperB, LOW);
  digitalWrite(MS2StepperB, LOW);
  digitalWrite(MS3StepperB, LOW);

  // set initial direction
  digitalWrite(setDirStepperB, HIGH);
  
  // enable (active low)
  digitalWrite(enableStepperB, LOW);

  Serial.begin(9600);

  stepper.setMaxSpeed(400); // 200 steps/sec = 1rev/sec = 60rpm
  stepper.setAcceleration(100);
  stepper.moveTo(400); // seems to behave as 100/turn for this
}
void loop()
{
    if (stepper.distanceToGo() == 0)
      // change direction once position is reached
      stepper.moveTo(-stepper.currentPosition());
    stepper.run();
}
