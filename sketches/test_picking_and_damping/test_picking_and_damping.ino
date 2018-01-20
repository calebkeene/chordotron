// -*- mode: C++ -*-
// Make a single servo damp the string before each pick from the stepper motor.
//
#include <AccelStepper.h>
#include <Servo.h>

// Stepper motor driver B
const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepStepperB = 9;
const int setDirStepperB = 8;

const int maxStepperSpeed = 1000;
const int numberSteps = 200;
const int numberPickers = 4;

// Servo
const int init_pos = 120;
const int change_angle = 5;

const int damper_delay = 100;

AccelStepper stepper(1, stepStepperB, setDirStepperB);

Servo damper;    // create servo object to control the damper

void setup()
{  

  // set up Stepper Motor B
  //
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

  // Set up servo on pin 21
  //
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
  if(stepper.distanceToGo() == 0) {
    Serial.println("one revolution completed, resetting");
    Serial.print("current position is: ");
    Serial.println(stepper.currentPosition());
    stepper.setCurrentPosition(0);
    Serial.print("set current position to: ");
    Serial.println(stepper.currentPosition());
    stepper.moveTo(numberSteps/numberPickers);
    stepper.setMaxSpeed(maxStepperSpeed);
    stepper.setSpeed(maxStepperSpeed);
    delay(damper_delay);
    use_damper2();
    Serial.println("damp");
    delay(damper_delay);
    reset_damper();
    Serial.println("reset damper");
    Serial.println();
    delay(damper_delay);
  }
//    if (stepper.distanceToGo() == 0){
//      // change direction once position is reached
//      stepper.moveTo(-stepper.currentPosition());
//    }
  stepper.runSpeedToPosition();
}

