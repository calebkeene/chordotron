const int enableStepperA = 48; 
const int MS1StepperA = 49; 
const int MS2StepperA = 50;
const int MS3StepperA = 51;
const int stepA_pin = 52;
const int dirA_pin = 53;

const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepB_pin = 9;
const int dirB_pin = 8;
int stepsToMove = 100;

bool moveSteppers;

void setup() {
 // put your setup code here, to run once:
  pinMode(MS1StepperA, OUTPUT);
  pinMode(MS2StepperA, OUTPUT);
  pinMode(MS3StepperA, OUTPUT);
  pinMode(enableStepperA, OUTPUT);
  pinMode(stepA_pin, OUTPUT);
  pinMode(dirA_pin, OUTPUT);


  digitalWrite(MS1StepperA, LOW);
  digitalWrite(MS2StepperA, LOW);
  digitalWrite(MS3StepperA, LOW);
  digitalWrite(enableStepperA, LOW);
  digitalWrite(dirA_pin, LOW);

  pinMode(MS1StepperB, OUTPUT);
  pinMode(MS2StepperB, OUTPUT);
  pinMode(MS3StepperB, OUTPUT);
  pinMode(enableStepperB, OUTPUT);
  pinMode(stepB_pin, OUTPUT);
  pinMode(dirB_pin, OUTPUT);

  digitalWrite(MS1StepperB, LOW);
  digitalWrite(MS2StepperB, LOW);
  digitalWrite(MS3StepperB, LOW);
  digitalWrite(enableStepperB, LOW);
  digitalWrite(dirA_pin, HIGH);

  moveSteppers = true;
}

void loop() {
 // put your main code here, to run repeatedly:
 if (moveSteppers) {
  for(int i = 0; i < stepsToMove; i++) {
   digitalWrite(stepA_pin, HIGH);
   digitalWrite(stepB_pin, HIGH);
   delayMicroseconds(505);
   digitalWrite(stepA_pin, LOW);
   digitalWrite(stepB_pin, LOW);
   delayMicroseconds(505);
  }
  moveSteppers = false;
 }
}
