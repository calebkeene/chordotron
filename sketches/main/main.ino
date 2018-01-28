// Implement stepper motor as a fretter
// Move stpper motor to specified frets as defined by a look-up table

#include <AccelStepper.h>
#include <Servo.h>
#include <MIDI.h>

// Stepper motor driver A
const int enableStepperA = 48; 
const int MS1StepperA = 49; 
const int MS2StepperA = 50;
const int MS3StepperA = 51;
const int stepStepperA = 52;
const int setDirStepperA = 53;

//Stepper motor driver B
const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepStepperB = 9;
const int setDirStepperB = 8;

const int damperServoPin = 21;
const int fretterServoPin = 20;

const int damperOffPos = 127;
const int damperOnPos = 135;

const int fretterOffPos = 0;// placeholders just for testing
const int fretterOnPos = 45;

const int maxStepperSpeed = 1000;
const int totalSteps = 200;
const int noteSteps = 40;
const int halvedNoteSteps = 20;

const double scale_length = 816.00;     // in mm
const int numberFrets = 13;

// length from nut to bridge = 816 mm
// max length travelled by stepper = 450 mm

int numberNotesPlayed = 0;
int stepsToTake = 0;

bool playingNote = false;
bool frettingNote = false;

double fretPositions[numberFrets];

Servo damper;
Servo fretter;
AccelStepper fretterStepper(1, stepStepperA, setDirStepperA);
AccelStepper pickerStepper(2, stepStepperB, setDirStepperB);
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);

// d = s - (s/(2^(n/12)))
// d = distance of fret from nut (mm)
// s = scale length (mm)
// n = fret number
void makeFretPositions() {
  for (int n = 1; n < (numberFrets - 1); n++) {
    fretPositions[n-1] = scale_length - (scale_length/pow(2.0, (n/12.0)));
  }
}

void setup()
{  
  // midi1.setHandleNoteOn(noteOnHandler);
  // midi1.setHandleNoteOff(noteOffHandler);
  // midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages

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
  
  makeFretPositions();

  Serial.begin(9600);
}

// void noteOnHandler(byte channel, byte pitch, byte velocity) {
//   Serial.println("running noteOnHandler");
//   Serial.print("channel: ");
//   Serial.println(channel);
//   Serial.print("pitch: ");
//   Serial.println(pitch);
//   Serial.print("velocity: ");
//   Serial.println(velocity);

//   if(velocity > 0) {
//     int fret = convertPitchToFret(pitch);
//     fretNote(fret);
//   }
// }

// void convertPitchToFret(byte pitch) {
//   // lookup table of some kind here
//   return fret;
// }

// void noteOffHandler(byte channel, byte pitch, byte velocity) {
//   Serial.println("This will be where the magic happens");
// }

void dampNote() {
  Serial.println("damping note");
  applyServoEffector(damper, 200, damperOnPos);
  applyServoEffector(damper, 200, damperOffPos);
}

void applyServoEffector(Servo servo, int delayTime, int position) {
  delay(delayTime);
  servo.write(position);
}

// wheel diameter = 22.3 mm
// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm
// distance travelled in one step = 70.022/200 = 0.35011 mm
void fretNote(int fret) {
  double fretPosition = fretPositions[fret - 1];
  stepsToTake = fretPosition/0.35011;
  
  Serial.println("Fretting note!");
  Serial.print("moving to fret ");
  Serial.println(fret);
  Serial.print("fretPosition: ");
  Serial.println(fretPosition);
  Serial.print("stepsToTake: ");
  Serial.println(stepsToTake);

  fretterStepper.moveTo(stepsToTake);
  fretterStepper.setSpeed(maxStepperSpeed);
  frettingNote = true;
}

void playNote() {
  Serial.println("playing note!");
  Serial.print("noteSteps: ");
  Serial.println(noteSteps);
  pickerStepper.moveTo(noteSteps);
  pickerStepper.setSpeed(maxStepperSpeed);
  playingNote = true;

  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
}

void checkForNote() {
  if (Serial.available() > 0) {
    int fretToMoveTo = Serial.parseInt();
    fretNote(fretToMoveTo);
  }
}

void loop() {
  if(frettingNote && fretterStepper.distanceToGo() == 0) {
    Serial.println("reached fret position");
    frettingNote = false;

    Serial.println("applying fretter");
    applyServoEffector(fretter, 0, fretterOnPos);

    playNote();
  }

  if (playingNote && !frettingNote && pickerStepper.distanceToGo() == 0) {
    Serial.print("picker current position: ");
    Serial.println(pickerStepper.currentPosition());
    Serial.println("reached play note position");

    dampNote();

    Serial.println("removing fretter");
    applyServoEffector(fretter, 0, fretterOffPos);

    numberNotesPlayed += 1;
    pickerStepper.setCurrentPosition(0);
    playingNote = false;
  }

  //midi1.read();
  checkForNote();
  fretterStepper.runSpeedToPosition();
  pickerStepper.runSpeedToPosition();
}

