#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>
#include <MIDI.h>
#include <HashMap.h>

// Stepper motor driver A
#define enableStepperA 48 
#define MS1StepperA 49 
#define MS2StepperA 50
#define MS3StepperA 51
#define stepStepperA 52
#define setDirStepperA 53

//Stepper motor driver B
#define enableStepperB 13 
#define MS1StepperB 12 
#define MS2StepperB 11
#define MS3StepperB 10
#define stepStepperB 9
#define setDirStepperB 8

#define damperServoPin 21
#define fretterServoPin1 20       // fretter 1 (fretters A and B)
#define fretterServoPin2 19       // fretter 2 (fretters C and D)

#define damperOffPos 127
#define damperOnPos 135

// placeholder just for testing
#define fretterOffPos 45
#define fretterACOnPos 15    // use fretter A/C on fretter 1/2
#define fretterBDOnPos 65    // use fretter B/D on fretter 1/2

#define maxStepperSpeed 1000
#define totalSteps 200
#define noteSteps 40
#define halvedNoteSteps 20
#define numberNotes 20

const double scaleLength = 816.00;     // in mm
const double initialFretPosition = 0.0;   // initial position of the fretting mechanism in mm

// length from nut to bridge = 816 mm
// max length travelled by stepper = 450 mm

int numberNotesPlayed = 0;
int currentStepsTakenFretterA = 0;
int currentStepsTakenFretterB = 0;
int currentStepsTakenFretterC = 0;
int currentStepsTakenFretterD = 0;

bool playingNote = false;
bool frettingNote = false;

//double noteFreqTable[127];
HashType<byte, int>hashRawPitchStepsFretterA[numberNotes];
HashMap<byte, int> pitchStepsTableFretterA = HashMap<byte, int> (hashRawPitchStepsFretterA, numberNotes);
HashType<byte, int>hashRawPitchStepsFretterBC[numberNotes];
HashMap<byte, int> pitchStepsTableFretterBC = HashMap<byte, int> (hashRawPitchStepsFretterBC, numberNotes);
HashType<byte, int>hashRawPitchStepsFretterD[numberNotes];
HashMap<byte, int> pitchStepsTableFretterD = HashMap<byte, int> (hashRawPitchStepsFretterD, numberNotes);

Servo damper;
Servo fretter1, fretter2;
AccelStepper fretterStepper1(1, stepStepperA, setDirStepperA);
AccelStepper fretterStepper2(1, stepStepperB, setDirStepperB);
MultiStepper steppers;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);

/**
 * The initial fret position is determined by the positions of the fretting mechanism.
 * This needs to be accounted for when calculating the fret positions for the string.
 * d = s - (s/(2^(n/12)))
 * d = distance of fret from nut (mm)
 * s = scale length (mm)
 * n = fret number
 * a = distance from fret starting position to nut
 * 
 * Assume the string is tuned to A3 (220 Hz)
 * Theoretically, should be able to play 20 notes on a single string (including open string)
 * A3 (pitch 45) --> F5 (pitch 65)
 * Use pitch value recieved pitch value from MIDI and use it to find the number of steps
 * away from the starting position.
 * 
 * The initial fret position is determined by the positions of the fretting mechanism.
 * This needs to be accounted for when calculating the fret positions for the string.
 * d = s - (s/(2^(n/12)))
 * d = distance of fret from nut (mm)
 * s = scale length (mm)
 * n = fret number
 * a = distance from fret starting position to nut
 */
void makePitchStepsTables() {
  double distanceFromNutA = 25;
  double distanceFromNutBC = 65;
  double distanceFromNutD = 105;
  byte pitch = 45;

  // set initial step positions
  currentStepsTakenFretterA = distanceFromNutA/0.35011;
  currentStepsTakenFretterB = distanceFromNutBC/0.35011;
  currentStepsTakenFretterC = currentStepsTakenFretterB;
  currentStepsTakenFretterD = distanceFromNutD/0.35011;

  // make tables
  for (int n = 0; n < numberNotes; n++) {
    double fretPosition = scaleLength - (scaleLength/pow(2.0, (n/12.0)));
    
    int stepsToFretA = (fretPosition - distanceFromNutA)/0.35011;
    pitchStepsTableFretterA[n](pitch + n, stepsToFretA);

    int stepsToFretBC = (fretPosition - distanceFromNutBC)/0.35011;
    pitchStepsTableFretterBC[n](pitch + n, stepsToFretBC);

    int stepsToFretD = (fretPosition - distanceFromNutD)/0.35011;
    pitchStepsTableFretterD[n](pitch + n, stepsToFretD);
  }
}

void setup()
{  
   midi1.setHandleNoteOn(noteOnHandler);
   midi1.setHandleNoteOff(noteOffHandler);
   midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages

  damper.attach(damperServoPin);
  fretter1.attach(fretterServoPin1);
  fretter2.attach(fretterServoPin2);

  // Stepper A => Fretter1
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

  // Stepper B => Fretter2
  pinMode(MS1StepperB, OUTPUT);
  pinMode(MS2StepperB, OUTPUT);
  pinMode(MS3StepperB, OUTPUT);
  pinMode(enableStepperB, OUTPUT);
  pinMode(stepStepperB, OUTPUT);
  pinMode(setDirStepperB, OUTPUT);
  
  digitalWrite(MS1StepperB, LOW);
  digitalWrite(MS2StepperB, LOW);
  digitalWrite(MS3StepperB, LOW);

  digitalWrite(setDirStepperB, HIGH);
  digitalWrite(enableStepperB, LOW);

  fretterStepper1.setMaxSpeed(maxStepperSpeed);
  fretterStepper1.setSpeed(maxStepperSpeed);

  fretterStepper2.setMaxSpeed(maxStepperSpeed);
  fretterStepper2.setSpeed(maxStepperSpeed);

  // this will be done with an automated calibration sequence in the future
  fretterStepper1.setCurrentPosition(0);
  
  damper.write(damperOffPos);
  fretter1.write(fretterOffPos);
  fretter2.write(fretterOffPos);

  // use MultiStepper library to handle both stepper motors
  steppers.addStepper(fretterStepper2);
  steppers.addStepper(fretterStepper1);
  
  makePitchStepsTables();

  Serial.begin(9600);
}

void noteOnHandler(byte channel, byte pitch, byte velocity) {
  Serial.println("running noteOnHandler");
  Serial.print("channel: ");
  Serial.println(channel);
  Serial.print("pitch: ");
  Serial.println(pitch);
  Serial.print("velocity: ");
  Serial.println(velocity);

  if(velocity > 0) {
    fretNote(pitch);
  }

  playNote(maxStepperSpeed);
}

void noteOffHandler(byte channel, byte pitch, byte velocity) {
  dampNote();
}

void dampNote() {
  Serial.println("damping note");
  applyServoEffector(damper, 200, damperOnPos);
  applyServoEffector(damper, 0, damperOffPos);
  Serial.println();
}

void applyServoEffector(Servo servo, int delayTime, int position) {
  delay(delayTime);
  servo.write(position);
}

// wheel diameter = 22.3 mm
// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm
// distance travelled in one step = 70.022/200 = 0.35011 mm
void fretNote(byte pitch) {
  // check current positions of both fretters
  // find steps to fret for both fretters
  // use fretter with smallest distance between fret and current position

  int stepsToFretFretterA = pitchStepsTableFretterA.getValueOf(pitch);
  int stepsToFretFretterBC = pitchStepsTableFretterBC.getValueOf(pitch);
  int stepsToFretFretterD = pitchStepsTableFretterD.getValueOf(pitch);
  
  int stepsToTakeFretterA = stepsToFretFretterA - currentStepsTakenFretterA;
  int stepsToTakeFretterB = stepsToFretFretterBC - currentStepsTakenFretterB;
  int stepsToTakeFretterC = stepsToFretFretterBC - currentStepsTakenFretterC;
  int stepsToTakeFretterD = stepsToFretFretterD - currentStepsTakenFretterD;
  int stepsToTake = 0;
  int dirStepper = 0;
  char stepper = 'A';

  int smallestDistance = min(stepsToTakeFretterB, stepsToTakeFretterC);

  if (smallestDistance == stepsToTakeFretterC) {
    smallestDistance = min(stepsToTakeFretterC, stepsToTakeFretterD);
    dirStepper = setDirStepperB;

    if (stepsToTakeFretterC < stepsToTakeFretterD) {
      // indicate that fretter 2 needs to be set to fretterACOnPos
      stepper = 'C';
    } else {
      // indicate that fretter 2 needs to be set to fretterBDOnPos
      stepper = 'D';
    }

    stepsToTake = stepsToTakeFretterC;
    currentStepsTakenFretterC = stepsToFretFretterBC;
    currentStepsTakenFretterD = stepsToFretFretterD;
  } else {
    smallestDistance = min(stepsToTakeFretterA, stepsToTakeFretterB);
    dirStepper = setDirStepperA;

    if (smallestDistance == stepsToTakeFretterB) {
      // indicate that fretter 2 needs to be set to fretterACOnPos
      stepper = 'B';
    } else {
      // indicate that fretter 2 needs to be set to fretterBDOnPos
      stepper = 'A';
    }

    stepsToTake = stepsToTakeFretterB;
    currentStepsTakenFretterA = stepsToFretFretterA;
    currentStepsTakenFretterB = stepsToFretFretterBC;
  }

  if (stepsToTake < 0) {
    digitalWrite(dirStepper, LOW);
    Serial.println("fretting away from bridge");
  } else {
    digitalWrite(dirStepper, HIGH);
    Serial.println("fretting to bridge");
  }

  moveStepper(stepper, abs(stepsToTake));
  
  frettingNote = true;
}

/**
 * In some cases the movement of the a fretter may be impeded by the current position
 * of the other fretter. In these cases both fretters will need to be moved.
 * 
 * i.e. In their starting positions fretters are right next to each other. In order to play
 *      the first fret fretter 1 will need to move to the right. Fretter 2 will first need
 *      to be moved out of its way.
 */
void calculateFretterPositions() {
}

void moveStepper(char stepper, int steps) {
  Serial.print("stepper: ");
  Serial.println(stepper);
  Serial.print("stepsToTake: ");
  Serial.println(steps);
  switch (stepper) {
    case 'A':
      // indicate that fretter 1 needs to be set to fretterACOnPos
      fretterStepper1.moveTo(steps);
      fretterStepper1.setSpeed(maxStepperSpeed);
      break;
    case 'B':
      // indicate that fretter 1 needs to be set to fretterBDOnPos
      fretterStepper1.moveTo(steps);
      fretterStepper1.setSpeed(maxStepperSpeed);
      break;
    case 'C':
      // indicate that fretter 2 needs to be set to fretterACOnPos
      fretterStepper2.moveTo(steps);
      fretterStepper2.setSpeed(maxStepperSpeed);
      break;
    default:
      // indicate that fretter 2 needs to be set to fretterBDOnPos
      fretterStepper2.moveTo(steps);
      fretterStepper2.setSpeed(maxStepperSpeed);
      break;
  }
}

void playNote(int stepperSpeed) {
//  pickerStepper.moveTo(noteSteps);
//  pickerStepper.setSpeed(stepperSpeed);
  playingNote = true;
  Serial.println("playing note");

  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
}

void loop() {
  midi1.read();

  if (frettingNote) {
    while (fretterStepper1.distanceToGo() > 0) {
      fretterStepper1.runSpeedToPosition();
    }

    if (fretterStepper1.distanceToGo() == 0) {
      frettingNote = false;
    }
  }

  if (playingNote) {
//    while (pickerStepper.distanceToGo() > 0) {
//      pickerStepper.runSpeedToPosition();
//    }
//
//    if (pickerStepper.distanceToGo() == 0) {
//      playingNote = false;
//      pickerStepper.setCurrentPosition(0);
//    }
      playingNote = false;
  }
}
