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

// Stepper motor driver B
const int enableStepperB = 13; 
const int MS1StepperB = 12; 
const int MS2StepperB = 11;
const int MS3StepperB = 10;
const int stepStepperB = 9;
const int setDirStepperB = 8;

const int damperServoPin = 21;

const int fretterServoPin = 20;

const int maxStepperSpeed = 1000;
const int totalSteps = 200;
const int noteSteps = 40;
const int halvedNoteSteps = 20;

const int damperOffPosTape = 128;
const int damperOffPosFoam = 127;
const int damperOffPos = damperOffPosTape;
const int damperOnPos = 135;

int numberNotesPlayed = 0;

const double scale_length = 816.00;     // in mm
const int numberFrets = 13;

const int fretOffPos = 180;
const int fretOnPos = 90;

// length from nut to bridge = 816 mm
// max length travelled by stepper = 450 mm

int currentSteps = 0;     // number of steps away from the starting positions
int stepsToTake = 0;      // number of steps to take from the current position to the next fret
int fretNum = 1;

bool playingNote = false;
bool frettingString = false;

double fretPositions[numberFrets];

int test = 0;

Servo damper;
Servo fretterServo;
AccelStepper pickWheelStepper(1, stepStepperB, setDirStepperB);
AccelStepper fretterStepper(1, stepStepperA, setDirStepperA);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);

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
  midi1.setHandleNoteOn(noteOnHandler);
  midi1.setHandleNoteOff(noteOffHandler);

  midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages

  damper.attach(damperServoPin);

  fretterServo.attach(fretterServoPin);

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
  digitalWrite(setDirStepperB, HIGH);
  
  // enable (active low)
  digitalWrite(enableStepperB, LOW);

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


  Serial.begin(9600);

  pickWheelStepper.setMaxSpeed(maxStepperSpeed);
  pickWheelStepper.setSpeed(maxStepperSpeed);
  fretterStepper.setMaxSpeed(maxStepperSpeed);
  fretterStepper.setSpeed(maxStepperSpeed);
  damper.write(damperOffPos);
  fretterServo.write(fretOffPos);

  makeFretPositions();
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
    playNote();
  }
}

void noteOffHandler(byte channel, byte pitch, byte velocity) {
  Serial.println("running noteOffHandler");
}

void dampNote() {
  delay(200);
  damper.write(damperOnPos);
  delay(200);
  damper.write(damperOffPos);
}

void removeFret(){
  fretterServo.write(fretOffPos);
}

void placeFret(){
  fretterServo.write(fretOnPos);
}

void playNote() {
  //Serial.println("running playNote");
  pickWheelStepper.moveTo(noteSteps);
  playingNote = true;
  numberNotesPlayed += 1;

  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
  pickWheelStepper.setSpeed(maxStepperSpeed);
}

void checkForNote() {
//  if(analogRead(A10) > 1000 && !playingNote) {
//    playNote();
//  }

  if (test == 0) {
    test = 1;
    setFretterPosition();
    playNote();
  }

//  if (Serial.available() > 0) {
//    setFretterPosition();
//    playNote();
//  } else {
//    playingNote = false;
//    frettingString = false;
//  }
}

// wheel diamter = 22.3 mm
// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm
// distance travelled in one step = 70.022/200 = 0.35011 mm
// steps to take from the starting position to the next fret
int stepsToFret(int n) {
  int steps = 0;
  if (n > 0) {
    double toPos = fretPositions[n - 1];
    steps = toPos/0.35011;
  }
  return steps;
}

void setFretterPosition(){
  frettingString = true;
  int steps = stepsToFret(fretNum) - currentSteps;
  currentSteps += stepsToTake;
  
  if (steps < 0) {
    digitalWrite(setDirStepperB, HIGH);
  } else {
    digitalWrite(setDirStepperB, LOW);
  }

  stepsToTake = abs(steps);
  fretterStepper.moveTo(stepsToTake);
  fretterStepper.setSpeed(maxStepperSpeed);
}

void loop() {
  if (fretterStepper.distanceToGo() == 0 && frettingString) {
    placeFret();
    fretterStepper.setCurrentPosition(0);
    frettingString = false;
  }

  if (pickWheelStepper.distanceToGo() == 0 && playingNote && !frettingString) {
      Serial.println("pick");
      removeFret();
      dampNote();
      pickWheelStepper.setCurrentPosition(0);
      playingNote = false;
    }

//  midi1.read();
  checkForNote();

  if (frettingString) {
    fretterStepper.runSpeedToPosition();
  }
  if (playingNote) {
    pickWheelStepper.runSpeedToPosition();
  }
  Serial.println();
}

