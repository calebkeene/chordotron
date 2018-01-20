// -*- mode: C++ -*-
// Make a single stepper bounce from one limit to another
//
#include <AccelStepper.h>
#include <Servo.h>

// Stepper motor driver A
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

const int damperServoPin = 21;

const int maxStepperSpeed = 1000;
const int numberSteps = 200;

const int damperOffPos = 125;
const int damperOnPos = 135;

AccelStepper stepper(1, stepStepperA, setDirStepperA);
Servo damper;

void setup()
{  
  damper.attach(damperServoPin);

  // Stepper A setup
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


  // Stepper B setup
  // pinMode(MS1StepperB, OUTPUT);
  // pinMode(MS2StepperB, OUTPUT);
  // pinMode(MS3StepperB, OUTPUT);
  // pinMode(enableStepperB, OUTPUT);
  // pinMode(stepStepperB, OUTPUT);
  // pinMode(setDirStepperB, OUTPUT);

  // // microstep resolution
  // digitalWrite(MS1StepperB, LOW);
  // digitalWrite(MS2StepperB, LOW);
  // digitalWrite(MS3StepperB, LOW);

  // // set initial direction
  // digitalWrite(setDirStepperB, HIGH);
  
  // // enable (active low)
  // digitalWrite(enableStepperB, LOW);

  Serial.begin(9600);
  stepper.moveTo(numberSteps);

  stepper.setMaxSpeed(400); // 200 steps/sec = 1rev/sec = 60rpm
  stepper.setAcceleration(100);

  damper.write(damperOffPos);
}

void setDamperOn() {
  Serial.println("setting damper ON");
  damper.write(damperOnPos);
  delay(1000);
}

void setDamperOff() {
  Serial.println("setting damper OFF");
  damper.write(damperOffPos);
  delay(1000);
}

void loop() {
  if(stepper.distanceToGo() == 0) {
    Serial.println("one revolution completed, resetting");
    Serial.print("current position is: ");
    Serial.println(stepper.currentPosition());
    stepper.setCurrentPosition(0);
    Serial.print("set current position to: ");
    Serial.println(stepper.currentPosition());
    stepper.moveTo(numberSteps);
    stepper.setMaxSpeed(maxStepperSpeed);
    stepper.setSpeed(maxStepperSpeed);

    setDamperOn();
    setDamperOff();
  }

  stepper.run();
  //stepper.runSpeedToPosition();
}

