// Implement stepper motor as a fretter
// Move stpper motor to specified frets as defined by a look-up table

#include <AccelStepper.h>
#include <Servo.h>
#include <MIDI.h>

// Stepper motor driver B
//const int enableStepper = 13; 
//const int MS1Stepper = 12; 
//const int MS2Stepper = 11;
//const int MS3Stepper = 10;
//const int stepStepper = 9;
//const int setDirStepper = 8;

// Stepper motor driver A
const int enableStepper = 48; 
const int MS1Stepper = 49; 
const int MS2Stepper = 50;
const int MS3Stepper = 51;
const int stepStepper = 52;
const int setDirStepper = 53;

const int fretterServoPin = 20;

const int maxStepperSpeed = 1000;
const int totalSteps = 200;
const int noteSteps = 40;
const int halvedNoteSteps = 20;

const double scale_length = 816.00;     // in mm
const int numberFrets = 13;

const int fretOffPos = 180;
const int fretOnPos = 90;

// length from nut to bridge = 816 mm
// max length travelled by stepper = 450 mm

int numberNotesPlayed = 0;
int currentSteps = 0;     // number of steps away from the starting positions
int stepsToTake = 0;      // number of steps to take from the current position to the next fret
int fretNum = 1;

bool playingNote = false;

double fretPositions[numberFrets];

Servo fretterServo;
AccelStepper fretterStepper(1, stepStepper, setDirStepper);
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

  fretterServo.attach(fretterServoPin);

  pinMode(MS1Stepper, OUTPUT);
  pinMode(MS2Stepper, OUTPUT);
  pinMode(MS3Stepper, OUTPUT);
  pinMode(enableStepper, OUTPUT);
  pinMode(stepStepper, OUTPUT);
  pinMode(setDirStepper, OUTPUT);

  // microstep resolution
  digitalWrite(MS1Stepper, LOW);
  digitalWrite(MS2Stepper, LOW);
  digitalWrite(MS3Stepper, LOW);

  // set initial direction
  digitalWrite(setDirStepper, HIGH);
  
  // enable (active low)
  digitalWrite(enableStepper, LOW);


  Serial.begin(9600);

  fretterStepper.setMaxSpeed(maxStepperSpeed);
  fretterStepper.setSpeed(maxStepperSpeed);
  fretterServo.write(fretOffPos);

  makeFretPositions();
}

void removeFret(){
  fretterServo.write(fretOffPos);
}

void placeFret(){
  fretterServo.write(fretOnPos);
}

// move fretter back to starting position
void resetFretter(){
  digitalWrite(setDirStepper, LOW);
  fretterStepper.moveTo(currentSteps);
}

void setStepperSpeed(int speed) {
  fretterStepper.setSpeed(speed);
}

void noteOnHandler(byte channel, byte pitch, byte velocity) {
  if(velocity > 0) {
    playNote();
  }
}

void noteOffHandler(byte channel, byte pitch, byte velocity) {
  Serial.println("running noteOffHandler");
}

void playNote() {
  playingNote = true;
  numberNotesPlayed += 1;

  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
  setStepperSpeed(maxStepperSpeed);
}

void checkForNote() {
  if(analogRead(A10) > 1000 && !playingNote) {
    playNote();
  }
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
  playingNote = true;
  int steps = stepsToFret(fretNum) - currentSteps;
  currentSteps += stepsToTake;
  
  if (steps < 0) {
    digitalWrite(setDirStepper, LOW);
  } else {
    digitalWrite(setDirStepper, HIGH);
  }

  stepsToTake = abs(steps);
  fretterStepper.moveTo(stepsToTake);
}

void checkSerial() {
  if (Serial.available() > 0) {
    byte incomingByte = Serial.read();
    String incomingString = String(incomingByte);
    int n = incomingString.toInt() - 48;
    
    if (n > 0 && n < (numberFrets)) {
      fretNum = n;
    }
//    Serial.print("byte: ");
//    Serial.println(incomingByte);
//    Serial.print("string: ");
//    Serial.println(incomingString);
//    Serial.print("int: ");
//    Serial.println(n);

    setFretterPosition();
  }
}

void loop() {
  if (fretterStepper.distanceToGo() == 0 && playingNote) {
    placeFret();
    delay(500);
    fretterStepper.setCurrentPosition(0);
    Serial.println(fretterStepper.currentPosition());
    playingNote = false;
    removeFret();
  }

  Serial.println(fretNum);
  Serial.println();
  checkForNote();
  checkSerial();
  fretterStepper.runSpeedToPosition();
}

