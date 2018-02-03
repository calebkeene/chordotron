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
#define fretterServoPin 20

#define damperOffPos 127
#define damperOnPos 135

#define fretterOffPos 0   // placeholders just for testing
#define fretterOnPos 45

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
int currentStepsTaken = 0;
int stepsToTake = 0;

bool playingNote = false;
bool frettingNote = false;

//double noteFreqTable[127];
HashType<byte, int>hashRawPitchSteps[numberNotes];
HashMap<byte, int> pitchStepsTable = HashMap<byte, int> (hashRawPitchSteps, numberNotes);

Servo damper;
Servo fretter;
AccelStepper fretterStepper(1, stepStepperA, setDirStepperA);
AccelStepper pickerStepper(1, stepStepperB, setDirStepperB);
MultiStepper steppers;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);

/**
 * fn = f0*(a^n)
 * fn = frequnecy of note n half steps away from f0
 * f0 = fixed note frequency
 * n = number of half steps away from f0
 * a = 2^(1/12)
 * 
 * For simplicity, assume f0 = C0 (16.35 Hz)
 * where A1 is 21 helf steps away from C0
 */
//void makeNoteFreqTable() {
//  double a = pow(2, 1/12.0);
//  int n = 0;
//
//  for (n = 0; n < 128; n++) {
//    noteFreqTable[n] = 16.35*(pow(a, n));
//  }
//}

/**
 * The initial fret position is determined by the positions of the fretting mechanism.
 * This needs to be accounted for when calculating the fret positions for the string.
 * d = s - (s/(2^(n/12)))
 * d = distance of fret from nut (mm)
 * s = scale length (mm)
 * n = fret number
 * a = distance from fret starting position to nut
 */

/**
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
void makePitchStepsTable() {
  byte pitch = 45;

  for (int n = 0; n < numberNotes; n++) {
    double fretPosition = scaleLength - (scaleLength/pow(2.0, (n/12.0)));
    int stepsToFret = fretPosition/0.35011;
    pitchStepsTable[n](pitch + n, stepsToFret);
  }
}

void setup()
{  
   midi1.setHandleNoteOn(noteOnHandler);
   midi1.setHandleNoteOff(noteOffHandler);
   midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages

  damper.attach(damperServoPin);
  fretter.attach(fretterServoPin);

  // Stepper A => Fretter
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

  // Stepper B => Picker
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

  fretterStepper.setMaxSpeed(maxStepperSpeed);
  fretterStepper.setSpeed(maxStepperSpeed);

  pickerStepper.setMaxSpeed(maxStepperSpeed);
  pickerStepper.setSpeed(maxStepperSpeed);

  // this will be done with an automated calibration sequence in the future
  fretterStepper.setCurrentPosition(0);
  
  damper.write(damperOffPos);
  fretter.write(fretterOffPos);

  // use MultiStepper library to handle both stepper motors
  steppers.addStepper(pickerStepper);
  steppers.addStepper(fretterStepper);
  
  makePitchStepsTable();

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
    int stepsToFret = convertPitchToSteps(pitch);
    fretNote(stepsToFret);
  }

  playNote(maxStepperSpeed);
}

int convertPitchToSteps(byte pitch) {
  int steps = pitchStepsTable.getValueOf(pitch);

  if (!steps) {
    steps = 0;
  }

  return steps;
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
void fretNote(int stepsToFret) {
  Serial.print("steps to fret: ");
  Serial.println(stepsToFret);
  Serial.print("current steps taken: ");
  Serial.println(currentStepsTaken);
  stepsToTake = stepsToFret - currentStepsTaken;
  currentStepsTaken = stepsToFret;

  if (stepsToTake < 0) {
    digitalWrite(setDirStepperA, LOW);
    Serial.println("fretting away from bridge");
  } else {
    digitalWrite(setDirStepperA, HIGH);
    Serial.println("fretting to bridge");
  }

  Serial.print("steps to take: ");
  Serial.println(stepsToTake);

  fretterStepper.moveTo(abs(stepsToTake));
  fretterStepper.setSpeed(maxStepperSpeed);
  frettingNote = true;
}

void playNote(int stepperSpeed) {
  pickerStepper.moveTo(noteSteps);
  pickerStepper.setSpeed(stepperSpeed);
  playingNote = true;
  Serial.println("playing note");

  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
}

void loop() {
  midi1.read();

  if (frettingNote) {
    while (fretterStepper.distanceToGo() > 0) {
      fretterStepper.runSpeedToPosition();
    }

    if (fretterStepper.distanceToGo() == 0) {
      frettingNote = false;
    }
  }

  if (playingNote) {
    while (pickerStepper.distanceToGo() > 0) {
      pickerStepper.runSpeedToPosition();
    }

    if (pickerStepper.distanceToGo() == 0) {
      playingNote = false;
      pickerStepper.setCurrentPosition(0);
    }
  }
}
