#include <Servo.h>

#include <MIDI.h>



#define damperServoPin 21

#define fretter1ServoPin 20

#define fretter2ServoPin 40

#define fretter1InitPin A9

#define fretter2InitPin A10



#define damperOffPos 115

#define damperOnPos 105



#define fretter1OffPos 60

#define fretter1AOnPos 120

#define fretter1BOnPos 30

#define fretter2OffPos 60

#define fretter2AOnPos 30

#define fretter2BOnPos 100



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



StepperMotor fretterStepper1 = {48, 49, 50, 51, 52, 53}; // ORIGINAL FRETTER1

StepperMotor fretterStepper2 = {13, 12, 11, 10, 9, 8};

StepperMotor pickerStepper = {46, 0, 0, 0, 44, 42}; // MSCPin, MSBPin and MSCPin pulled LOW



int fretterStepper1APosition = 0;

int fretterStepper1BPosition = 0;

int fretterStepper2APosition = 0;

int fretterStepper2BPosition = 0;

int fretterStepper1Direction = HIGH;

int fretterStepper2Direction = LOW;



//int fretter1APositions[] = {0, 123};                                    // using bumbp switches

//int fretter1BPositions[] = {126, 236, 340, 438, 530, 617, 700, 778};    // using bumbp swicthes



int fretter1APositions[] = {0, 123};

int fretter1BPositions[] = {126, 236, 340, 438, 530, 617, 700, 778};

int fretter2APositions[] = {617, 548, 483, 421, 363, 308, 256, 207, 160, 117, 76, 37, 0};

int fretter2BPositions[] = {80};



bool playingNote = false;

bool frettingNote = false;

bool fretter1LastPlayedNearMiddle = false;

bool fretter2LastPlayedNearMiddle = false;



Servo damper;

Servo fretter1;

Servo fretter2;



MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);



void setup() {

  midi1.setHandleNoteOn(noteOnHandler);

  midi1.setHandleNoteOff(noteOffHandler);

  midi1.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming MIDI messages



  damper.attach(damperServoPin);

  fretter1.attach(fretter1ServoPin);

  fretter2.attach(fretter2ServoPin);



  // set up bump switches

  pinMode(fretter1InitPin, INPUT);

  pinMode(fretter2InitPin, INPUT);



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

  // pinMode(A10, OUTPUT);

  // pinMode(A9, OUTPUT);

  // pinMode(A8, OUTPUT);

  pinMode(pickerStepper.enablePin, OUTPUT);

  pinMode(pickerStepper.stepPin, OUTPUT);

  pinMode(pickerStepper.setDirectionPin, OUTPUT);

  // microstep resolution

  // digitalWrite(A10, LOW);

  // digitalWrite(A9, LOW);

  // digitalWrite(A8, LOW);

  // set initial direction

  digitalWrite(pickerStepper.setDirectionPin, LOW);

  // enable (active low)

  digitalWrite(pickerStepper.enablePin, LOW);



  damper.write(damperOffPos);

  fretter1.write(fretter1OffPos);

  fretter2.write(fretter2OffPos);

  resetInitPositions();



  Serial.begin(9600);

}



void dampNote() {

  Serial.println("damping note");

  applyServoEffector(damper, 200, damperOnPos);

  applyServoEffector(damper, 0, damperOffPos);

}



void fretNote(Servo fretter, int pos) {

  applyServoEffector(fretter, 75, pos);

}



void applyServoEffector(Servo servo, int delayTime, int pos) {

  servo.write(pos);

  delay(delayTime);

}



// wheel diameter = 22.3 mm

// distance travelled in one stepper motor revolution (200 steps) = 22.3(3.14) = 70.022 mm

// distance travelled in one step = 70.022/200 = 0.35011 mm

void playNote() {

  Serial.println("playing note");

  moveStepper(pickerStepper, noteSteps, LOW, 1000);

  //delay(100);



  //  while (playingNote) {

  //    moveStepper(pickerStepper, noteSteps, LOW, 1000);

  //    midi1.read();

  //  }

}



void noteOffHandler(byte channel, byte pitch, byte velocity) {

  Serial.println("note off event!");

  dampNote();

  disengageFretters();

  playingNote = false;

}



void noteOnHandler(byte channel, byte pitch, byte velocity) {

  Serial.println("note on handler!");

  playingNote = true;

  int note = (int)(pitch);

  Serial.print("pitch: ");

  Serial.println(note);

  if (velocity > 0) {

    if (note == startingNote) {

      // MIDI -> 48

      //open string => C3

      disengageFretters();

    }

    else if (note > startingNote && note <= 50) {

      //} else if (note > startingNote && note <= (startingNote + totalFretter1ANotes)) {

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

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction, 505);

      fretterStepper1APosition += stepsToMove;

      fretterStepper1BPosition += stepsToMove;

      // updateFretterPositions(fretterStepper1APosition, fretterStepper1BPosition, stepsToMove);



      fretNote(fretter1, fretter1AOnPos);

      //} else if (note > (startingNote + totalFretter1ANotes) && note <= (startingNote + totalFretter1ANotes + totalFretter1BNotes)) {

    }

    else if (note > 50 && note <= 58) {

      fretter1LastPlayedNearMiddle = (note == 58) ? true : false;

      if (fretter1LastPlayedNearMiddle && fretter2LastPlayedNearMiddle) {

        Serial.println("fretter 2 last played near middle, moving it out of the way");

        fretter2LastPlayedNearMiddle = false;

        // move fretter2 to 548 steps from origin (MIDI note == 60)

        // move from MIDI note 59 to 60 (617 steps to 548)

        int stepsToMove = 69;

        moveStepper(fretterStepper2, stepsToMove, HIGH, 505);

        fretterStepper2APosition -= stepsToMove;

        fretterStepper2BPosition -= stepsToMove;

      }

      Serial.print("fretter1LastPlayedNearMiddle: ");

      Serial.println(fretter1LastPlayedNearMiddle);

      int stepsFromOrigin = fretter1BPositions[note - 51];

      Serial.print("Steps: ");

      Serial.println(stepsFromOrigin);

      if (fretterStepper1BPosition < stepsFromOrigin) {

        fretterStepper1Direction = LOW; // go away from origin;

      } else {

        fretterStepper1Direction = HIGH;

      }

      int stepsToMove = stepsFromOrigin - fretterStepper1BPosition;

      if (note < 55) {

        stepsToMove += 20;

      }

      else {

        stepsToMove += 18;

      }

      moveStepper(fretterStepper1, abs(stepsToMove), fretterStepper1Direction, 505);

      fretNote(fretter1, fretter1BOnPos);

      fretterStepper1APosition += stepsToMove;

      fretterStepper1BPosition += stepsToMove;



    }

    else if (note > 58 && note <= 71) {

      fretter2LastPlayedNearMiddle = (note == 59) ? true : false;



      if (fretter1LastPlayedNearMiddle && fretter2LastPlayedNearMiddle) {

        Serial.println("fretter 1 last played near middle, moving it out of the way");

        fretter1LastPlayedNearMiddle = false;

        // move fretter2 to 548 steps from origin (MIDI note == 60)

        // move from MIDI note 59 to 60 (617 steps to 548)

        int stepsToMove = 78;

        moveStepper(fretterStepper1, stepsToMove, HIGH, 505);

        fretterStepper1APosition -= stepsToMove;

        fretterStepper1BPosition -= stepsToMove;

      }

      int stepsFromOrigin = fretter2APositions[note - 59];

      Serial.print("Steps: ");

      Serial.println(stepsFromOrigin);

      if (fretterStepper2APosition < stepsFromOrigin) {

        fretterStepper2Direction = LOW; // go away from origin;

      } else {

        fretterStepper2Direction = HIGH;

      }

      int stepsToMove = stepsFromOrigin - fretterStepper2APosition;

      moveStepper(fretterStepper2, abs(stepsToMove), fretterStepper2Direction, 505);



      fretNote(fretter2, fretter2AOnPos);

      fretterStepper2APosition += stepsToMove;

      fretterStepper2BPosition += stepsToMove;

    }

    else {

      // MIDI note 72

      int stepsFromOrigin = fretter2BPositions[0];

      Serial.print("Steps: ");

      Serial.println(stepsFromOrigin);

      if (fretterStepper2BPosition < stepsFromOrigin) {

        fretterStepper2Direction = LOW; // go away from origin;

      } else {

        fretterStepper2Direction = HIGH;

      }

      int stepsToMove = stepsFromOrigin - fretterStepper2BPosition;

      moveStepper(fretterStepper2, abs(stepsToMove), fretterStepper2Direction, 505);



      fretNote(fretter2, fretter2BOnPos);

      fretterStepper2APosition += stepsToMove;

      fretterStepper2BPosition += stepsToMove;

    }

  }

  playNote();

}



void resetInitPositions() {

  if (digitalRead(fretter1InitPin) == HIGH) {

    fretterStepper1APosition = 0;

    fretterStepper1BPosition = 0;

  }



  if (digitalRead(fretter2InitPin) == HIGH) {

    fretterStepper2APosition = 0;

    fretterStepper2BPosition = 0;

  }

}



void disengageFretters() {

  fretNote(fretter1, fretter1OffPos);

  fretNote(fretter2, fretter2OffPos);

}



void moveStepper(StepperMotor stepper, int stepsToMove, int stepperCurrentDirection, unsigned int delayTime) {

  digitalWrite(stepper.setDirectionPin, stepperCurrentDirection);

  for (int i = 0; i < stepsToMove; i++) {

    digitalWrite(stepper.stepPin, HIGH);

    delayMicroseconds(delayTime);

    digitalWrite(stepper.stepPin, LOW);

    delayMicroseconds(delayTime);

  }

}



void loop() {

  midi1.read();

}
