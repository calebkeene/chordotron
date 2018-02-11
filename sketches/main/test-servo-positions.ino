#include <Servo.h>
#include <MIDI.h>

#define damperServoPin 21
#define fretter1ServoPin 20
#define fretter2ServoPin 40

#define damperOffPos 115
#define damperOnPos 105

#define fretterOffPos 60
#define fretterOneArmA

#define fretterArmAOnPos 10
#define fretterArmBOnPos 110

// possible on position for the fretter on the other side OFF=110°, ON=130°
#define totalSteps 200
#define noteSteps 40

#define startingNote 48
#define totalFretter1ANotes 2
#define totalFretter1BNotes 8
#define totalFretter2ANotes 13
#define totalFretter2BNotes 1

struct StepperMotor {
  int enablePin;
  int MSAPin;
  int MSBPin;
  int MSCPin;
  int stepPin;
  int setDirectionPin;
};

StepperMotor fretterStepper1 = {48, 49, 50, 51, 52, 53};
StepperMotor fretterStepper2 = {13, 12, 11, 10, 9, 8};
StepperMotor pickerStepper = {44, 0, 0, 0, 42, 40};   // MSCPin, MSBPin and MSCPin pulled LOW

int fretterStepper1APosition = 0;
int fretterStepper1BPosition = 0;
int fretterStepper2APosition = 0;
int fretterStepper2BPosition = 0;

int fretterStepper1Direction = HIGH;
int fretterStepper2Direction = LOW;

int fretter1APositions[] = {71, 194};
int fretter1BPositions[] = {196, 306, 410, 508, 601, 688, 770, 848, 922, 991};
int fretter2APositions[] = {483, 421, 363, 308, 256, 207, 160, 117, 76, 37, 0};
//int fretter2APositions[] = {617, 548, 483, 421, 363, 308, 256, 207, 160, 117, 76, 37, 0};
int fretter2BPositions[] = {80};

bool playingNote = false;
bool frettingNote = false;

Servo damper;
Servo fretter1;
Servo fretter2;


void setup() {

  damper.attach(damperServoPin);
  fretter1.attach(fretter1ServoPin);
  fretter2.attach(fretter2ServoPin);

  // Stepper A => Fretter 1
  pinMode(fretterStepper1.MSAPin, OUTPUT);
  pinMode(fretterStepper1.MSBPin, OUTPUT);
  pinMode(fretterStepper1.MSCPin, OUTPUT);
  pinMode(fretterStepper1.enablePin, OUTPUT);
  pinMode(fretterStepper1.stepPin, OUTPUT);
  pinMode(fretterStepper1.setDirectionPin, OUTPUT);
  // microstep resolution
  digitalWrite(fretterStepper1.MSAPin, LOW);
  digitalWrite(fretterStepper1.MSBPin, LOW);
  digitalWrite(fretterStepper1.MSCPin, LOW);
  // set initial direction
  digitalWrite(fretterStepper1.setDirectionPin, fretterStepper1Direction);
  // enable (active low)
  digitalWrite(fretterStepper1.enablePin, LOW);

  // Stepper B => Fretter 2
  pinMode(fretterStepper2.MSAPin, OUTPUT);
  pinMode(fretterStepper2.MSBPin, OUTPUT);
  pinMode(fretterStepper2.MSCPin, OUTPUT);
  pinMode(fretterStepper2.enablePin, OUTPUT);
  pinMode(fretterStepper2.stepPin, OUTPUT);
  pinMode(fretterStepper2.setDirectionPin, OUTPUT);
  // microstep resolution
  digitalWrite(fretterStepper2.MSAPin, LOW);
  digitalWrite(fretterStepper2.MSBPin, LOW);
  digitalWrite(fretterStepper2.MSCPin, LOW);
  // set initial direction
  digitalWrite(fretterStepper2.setDirectionPin, fretterStepper2Direction);
  // enable (active low)
  digitalWrite(fretterStepper2.enablePin, LOW);

  // Stepper C => Picker
  pinMode(pickerStepper.enablePin, OUTPUT);
  pinMode(pickerStepper.stepPin, OUTPUT);
  pinMode(pickerStepper.setDirectionPin, OUTPUT);
  // set initial direction
  digitalWrite(pickerStepper.setDirectionPin, HIGH);
  // enable (active low)
  digitalWrite(pickerStepper.enablePin, LOW);

  damper.write(damperOffPos);
  fretter1.write(fretterOffPos);
  fretter2.write(fretterOffPos);

  Serial.begin(9600);
}

void checkForNote() {
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();
    Serial.print("Setting angle to ");
    Serial.println(angle);
    damper.write(angle);
  }
}


void loop() {
  checkForNote();
}

