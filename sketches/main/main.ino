#include <Servo.h>
#include <MIDI.h>

#define damperServoPin 21
#define fretter1ServoPin 20
#define fretter2ServoPin 40

#define damperOffPos 123
#define damperOnPos 120

#define fretterOffPos 60
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
int fretter1BPositions[] = {196, 306, 410, 508, 601, 688, 770, 848};
int fretter2APositions[] = {617, 548, 483, 421, 363, 308, 256, 207, 160, 117, 76, 37, 0};
int fretter2BPositions[] = {80};

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
//  fretter1.attach(fretter1ServoPin);
//  fretter2.attach(fretter2ServoPin);

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

void dampNote() {
  Serial.println("damping note");
  applyServoEffector(damper, 200, damperOnPos);
  applyServoEffector(damper, 0, damperOffPos);
}

void fretNote(Servo fretter, int pos) {
  fretter1.write(pos);
  fretter2.write(pos);
}

void applyServoEffector(Servo servo, int delayTime, int pos) {
  servo.write(pos);
  delay(delayTime);
}

// wheel diameter = 22.3 mm
// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm
// distance travelled in one step = 70.022/200 = 0.35011 mm

void playNote() {
  moveStepper(pickerStepper, noteSteps, HIGH);
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

  if (velocity > 0) {
    if (note == startingNote) {
      //open string => C4
      //moveToPosition(fretterStepper1, )

    } else if (note > startingNote && note <= (startingNote + totalFretter1ANotes)) {
      // A4# to C5#
      int stepsFromOrigin = fretter1APositions[note - (startingNote + 1)];
      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if (fretterStepper1APosition < stepsFromOrigin) {
        fretterStepper1Direction = LOW; // go away from origin;
      } else {
        fretterStepper1Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper1APosition;

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction);

      fretterStepper1APosition += stepsToMove;
      fretterStepper1BPosition += stepsToMove;
      //      updateFretterPositions(fretterStepper1APosition, fretterStepper1BPosition, stepsToMove);
      
      fretNote(fretter1, fretterArmAOnPos);
    } else if (note > (startingNote + totalFretter1ANotes) && note <= (startingNote + totalFretter1ANotes + totalFretter1BNotes)) {
      int stepsFromOrigin = fretter1BPositions[note - (startingNote + totalFretter1ANotes + 1)];
      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if (fretterStepper1BPosition < stepsFromOrigin) {
        fretterStepper1Direction = LOW; // go away from origin;
      } else {
        fretterStepper1Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper1BPosition;

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction);
      fretterStepper1APosition += stepsToMove;
      fretterStepper1BPosition += stepsToMove;
      
      fretNote(fretter1, fretterArmBOnPos);
    } else if (note > (startingNote + totalFretter1ANotes + totalFretter1BNotes) && note <= (startingNote + totalFretter1ANotes + totalFretter1BNotes + totalFretter2ANotes)) {
      int stepsFromOrigin = fretter2APositions[note - (startingNote + totalFretter1ANotes + totalFretter1BNotes + 1) ];

      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if (fretterStepper2APosition < stepsFromOrigin) {
        fretterStepper2Direction = LOW; // go away from origin;
      } else {
        fretterStepper2Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper2APosition;

      moveStepper(fretterStepper2, abs(stepsToMove), fretterStepper2Direction);
      fretterStepper2APosition += stepsToMove;
      fretterStepper2BPosition += stepsToMove;
      
      fretNote(fretter2, fretterArmAOnPos);
    } else if (note > (startingNote + totalFretter1ANotes + totalFretter1BNotes + totalFretter2ANotes) && note < (startingNote + totalFretter1ANotes + totalFretter1BNotes + totalFretter2ANotes + totalFretter2BNotes)) {
      int stepsFromOrigin = fretter2BPositions[note - (startingNote + totalFretter1ANotes + totalFretter1BNotes + totalFretter2ANotes + 1)];

      Serial.print("Steps: ");
      Serial.println(stepsFromOrigin);

      if (fretterStepper2BPosition < stepsFromOrigin) {
        fretterStepper2Direction = LOW; // go away from origin;
      } else {
        fretterStepper2Direction = HIGH;
      }

      int stepsToMove = stepsFromOrigin - fretterStepper2BPosition;

      moveStepper(fretterStepper2, abs(stepsToMove), fretterStepper2Direction);
      fretterStepper2APosition += stepsToMove;
      fretterStepper2BPosition += stepsToMove;
      
      fretNote(fretter2, fretterArmBOnPos);
    }
    playNote();
  }
}


void noteOffHandler(byte channel, byte pitch, byte velocity) {
  Serial.println("damp note");
}

//void updateFretterPositions(int * fretterA, int * fretterB, int stepsToMove) {
//  fretterA += stepsToMove;
//  fretterB += stepsToMove;
//}

void moveStepper(StepperMotor stepper, int stepsToMove, int stepperCurrentDirection) {
  fretNote(fretter1, fretterOffPos);
  fretNote(fretter2, fretterOffPos);
  
  digitalWrite(stepper.setDirectionPin, stepperCurrentDirection);

  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(stepper.stepPin, HIGH);
    delayMicroseconds(505);
    digitalWrite(stepper.stepPin, LOW);
    delayMicroseconds(505);
  }
}

void loop() {
  midi1.read();
}

