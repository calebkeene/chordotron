#include <Servo.h>
#include <MIDI.h>

#define damperServoPin 21
#define fretter1ServoPin 20
#define fretter2ServoPin 40

#define damperOffPos 123
#define damperOnPos 120

// possible on position for the fretter on the other side OFF=110°, ON=130°
#define totalSteps 200
#define noteSteps 40

#define startingNote 48

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
//StepperMotor pickerStepper = {13, 12, 11, 10, 9, 8};

int fretterStepper1APosition = 0;
int fretterStepper1BPosition = 0;
int fretterStepper2APosition = 0; // don't need 2B to reach top note

int fretterStepper1Direction = HIGH;
int fretterStepper2Direction = LOW;

int fretter1APositions[4] = {71, 194, 311, 421};
int fretter1BPositions[6] = {410, 508, 601, 688, 770, 848};
int fretter2APositions[13] = {617, 548, 483, 421, 363, 308, 256, 207, 160, 117, 76, 37, 0};

int fretter1OffPos = 45;

int fretterArm1AOnPos = 50;
int FretterArm1BOnPos = 45;
// Stepper motor driver A
// const int enableStepperA = 48; 
// const int MS1StepperA = 49; 
// const int MS2StepperA = 50;
// const int MS3StepperA = 51;
// const int stepStepperA = 52;
// const int setDirStepperA = 53;

// //Stepper motor driver B
// const int enableStepperB = 13; 
// const int MS1StepperB = 12; 
// const int MS2StepperB = 11;
// const int MS3StepperB = 10;
// const int stepStepperB = 9;
// const int setDirStepperB = 8;

bool playingNote = false;
bool frettingNote = false;

Servo damper;
Servo fretter1;
Servo fretter2;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);

void setup() {
  midi1.setHandleNoteOn(noteOnHandler);
  midi1.setHandleNoteOff(noteOffHandler);
  midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages

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


  // pinMode(pickerStepper.MSAPin, OUTPUT);
  // pinMode(pickerStepper.MSBPin, OUTPUT);
  // pinMode(pickerStepper.MSCPin, OUTPUT);
  // pinMode(pickerStepper.enablePin, OUTPUT);
  // pinMode(pickerStepper.stepPin, OUTPUT);
  // pinMode(pickerStepper.setDirectionPin, OUTPUT);
  // // microstep resolution
  // digitalWrite(pickerStepper.MSAPin, LOW);
  // digitalWrite(pickerStepper.MSBPin, LOW);
  // digitalWrite(pickerStepper.MSCPin, LOW);
  // // set initial direction
  // digitalWrite(pickerStepper.setDirectionPin, HIGH);
  // // enable (active low)
  // digitalWrite(pickerStepper.enablePin, LOW);
  
  damper.write(damperOffPos);
  fretter1.write(fretter1OffPos);
  fretter2.write(fretter1OffPos);

  Serial.begin(9600);
}

void dampNote() {
  Serial.println("damping note");
  applyServoEffector(damper, 200, damperOnPos);
  applyServoEffector(damper, 0, damperOffPos);
}

void fretNote(Servo fretter) {
  // applyServoEffector...
}

void applyServoEffector(Servo servo, int delayTime, int position) {
  delay(delayTime);
  servo.write(position);
}

// wheel diameter = 22.3 mm
// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm
// distance travelled in one step = 70.022/200 = 0.35011 mm

void playNote() {
//  moveStepper(pickerStepper, noteSteps, HIGH);
}

void noteOnHandler(byte channel, byte pitch, byte velocity) {
  // Serial.println("running noteOnHandler");
  // Serial.print("channel: ");
  // Serial.println(channel);
  // Serial.print("pitch: ");
  // Serial.println(pitch);
  // Serial.print("velocity: ");
  // Serial.println(velocity);

  int note = (int)(pitch);
  Serial.print("pitch: ");
  Serial.println(note);

  if(velocity > 0) {
    if(note == startingNote) {
      //open string => A3
      //moveToPosition(fretterStepper1, )

    }
    else if(note > startingNote && note <= (startingNote + 4)) {
      // A3# to C3#
      int stepsFromOrigin = fretter1APositions[note - startingNote - 1];
      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);
      
      if(fretterStepper1APosition < stepsFromOrigin) {
        fretterStepper1Direction = LOW; // go away from origin;
      }
      else {
        fretterStepper1Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper1APosition;

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction);
      
      fretterStepper1APosition += stepsToMove;
      fretterStepper1BPosition += stepsToMove;
      // fretNote();
    }
    else if(note > (startingNote + 4) && note <= (startingNote + 10)) {
      int stepsFromOrigin = fretter1BPositions[note - startingNote - 5];
      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if(fretterStepper1BPosition < stepsFromOrigin) {
        fretterStepper1Direction = LOW; // go away from origin;
      }
      else {
        fretterStepper1Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper1BPosition;

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction);
      fretterStepper1APosition += stepsToMove;
      fretterStepper1BPosition += stepsToMove;
      //fretNote();
    }
    else if(note > (startingNote + 10) && note <= (startingNote + 23)) {
      int stepsFromOrigin = fretter2APositions[note - startingNote - 10];

      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if(fretterStepper2APosition < stepsFromOrigin) {
        fretterStepper2Direction = LOW; // go away from origin;
      }
      else {
        fretterStepper2Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper2APosition;

      moveStepper(fretterStepper2, abs(stepsToMove), fretterStepper2Direction);
      fretterStepper2APosition += stepsToMove;
      //fretNote();
    }
    //playNote();
  }
}


void noteOffHandler(byte channel, byte pitch, byte velocity) {
  Serial.println("damp note");
}

//void checkForNote() {
//  if (Serial.available() > 0) {
//    Serial.println("moving stepper");
//    // USEFUL FOR DEBUGGING
//
//    int numberStepsToMove = Serial.parseInt();
//    moveStepper(fretterStepper1, numberStepsToMove, fretterStepper1Direction);
//
//  }
//}

void moveStepper(StepperMotor stepper, int stepsToMove, int stepperCurrentDirection) {

  digitalWrite(stepper.setDirectionPin, stepperCurrentDirection);

  for(int i = 0; i < stepsToMove; i++) {
    digitalWrite(stepper.stepPin, HIGH);
    delayMicroseconds(505);
    digitalWrite(stepper.stepPin, LOW);
    delayMicroseconds(505);
  }
}

void loop() {
  midi1.read();
//  checkForNote();
}

