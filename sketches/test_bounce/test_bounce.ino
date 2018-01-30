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

AccelStepper stepper1(1, stepStepperA, setDirStepperA);
AccelStepper stepper2(2, stepStepperB, setDirStepperB);

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

  stepper1.setMaxSpeed(400); // 200 steps/sec = 1rev/sec = 60rpm
  stepper1.setAcceleration(100);
  stepper1.moveTo(200); // seems to behave as 100/turn for this

  stepper2.setMaxSpeed(400); // 200 steps/sec = 1rev/sec = 60rpm
  stepper2.setAcceleration(100);
  stepper2.moveTo(200); // seems to behave as 100/turn for this
}
void loop()
{
    if (stepper1.distanceToGo() == 0){
      // change direction once position is reached
      Serial.println("changing direction");
      stepper1.moveTo(-stepper1.currentPosition());
    }
    if (stepper2.distanceToGo() == 0){
      // change direction once position is reached
      Serial.println("changing direction");
      stepper2.moveTo(-stepper2.currentPosition());
    }
    stepper1.run();
    stepper2.run();
}
