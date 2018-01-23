// -*- mode: C++ -*-
// Make a single servo damp the string before each pick from the stepper motor.
//
#include <AccelStepper.h>
#include <Servo.h>

// Stepper motor driver B
//const int enableStepperB = 13; 
//const int MS1StepperB = 12; 
//const int MS2StepperB = 11;
//const int MS3StepperB = 10;
//const int stepStepperB = 9;
//const int setDirStepperB = 8;

// Stepper motor driver A
const int enableStepperA = 48; 
const int MS1StepperA = 49; 
const int MS2StepperA = 50;
const int MS3StepperA = 51;
const int stepStepperA = 52;
const int setDirStepperA = 53;

const int maxStepperSpeed = 1000;
const int numberSteps = 200;
const int numberPickers = 5;

// Servo
const int init_pos = 125;
const int change_angle = 15;

const int damper_delay = 100;

AccelStepper stepper(1, stepStepperA, setDirStepperA);

Servo damper;    // create servo object to control the damper

void setup()
{  

  // set up Stepper Motor B
//  pinMode(MS1StepperB, OUTPUT);
//  pinMode(MS2StepperB, OUTPUT);
//  pinMode(MS3StepperB, OUTPUT);
//  pinMode(enableStepperB, OUTPUT);
//  pinMode(stepStepperB, OUTPUT);
//  pinMode(setDirStepperB, OUTPUT);

  // microstep resolution
//  digitalWrite(MS1StepperB, LOW);
//  digitalWrite(MS2StepperB, LOW);
//  digitalWrite(MS3StepperB, LOW);

// set initial direction
//  digitalWrite(setDirStepperB, HIGH);

// enable (active low)
//  digitalWrite(enableStepperB, LOW);

  // set up Stepper Motor A
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

  // Set up servo on pin 21
  damper.attach(21);

  Serial.begin(9600);
  stepper.moveTo(numberSteps);
  stepper.setMaxSpeed(maxStepperSpeed);
  stepper.setSpeed(maxStepperSpeed);
  damper.write(init_pos);
}

void reset_damper(){
  damper.write(init_pos);
}

void use_damper1() {
  damper.write(init_pos + change_angle);
}

void use_damper2(){
  damper.write(init_pos - change_angle);
}

void loop() {
//  if(stepper.distanceToGo() == 0) {
//    stepper.setCurrentPosition(0);
//    stepper.moveTo(numberSteps/numberPickers);
//    stepper.setMaxSpeed(maxStepperSpeed);
//    stepper.setSpeed(maxStepperSpeed);
//    delay(damper_delay);
//    use_damper2();
//    delay(damper_delay);
//    reset_damper();
//    delay(damper_delay);
//  }
//  
//  stepper.runSpeedToPosition();

  stepper.setCurrentPosition(0);
  stepper.moveTo(numberSteps/numberPickers);
  delay(damper_delay);
  use_damper2();
  delay(damper_delay);
  reset_damper();
  delay(damper_delay);
}

