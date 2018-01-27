//#include <AccelStepper.h>
//#include <Servo.h>
//#include <MIDI.h>
//
//const int enableStepperA = 48; 
//const int MS1StepperA = 49; 
//const int MS2StepperA = 50;
//const int MS3StepperA = 51;
//const int stepStepperA = 52;
//const int setDirStepperA = 53;
//
//const int damperServoPin = 21;
//
//const int maxStepperSpeed = 1000;
//const int totalSteps = 200;
//const int noteSteps = 40;
//const int halvedNoteSteps = 20;
//
//const int damperOffPos = 125;
//const int damperOnPos = 135;
//
//int numberNotesPlayed = 0;
//
//bool playingNote = false;
//
//Servo damper;
//AccelStepper stepper(1, stepStepperA, setDirStepperA);
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midi1);
//
//void setup()
//{  
//  midi1.setHandleNoteOn(noteOnHandler);
//  midi1.setHandleNoteOff(noteOffHandler);
//
//  midi1.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming MIDI messages
//
//  damper.attach(damperServoPin);
//
//  pinMode(MS1StepperA, OUTPUT);
//  pinMode(MS2StepperA, OUTPUT);
//  pinMode(MS3StepperA, OUTPUT);
//  pinMode(enableStepperA, OUTPUT);
//  pinMode(stepStepperA, OUTPUT);
//  pinMode(setDirStepperA, OUTPUT);
//
//  // microstep resolution
//  digitalWrite(MS1StepperA, LOW);
//  digitalWrite(MS2StepperA, LOW);
//  digitalWrite(MS3StepperA, LOW);
//
//  // set initial direction
//  digitalWrite(setDirStepperA, HIGH);
//  
//  // enable (active low)
//  digitalWrite(enableStepperA, LOW);
//
//
//  Serial.begin(9600);
//  //setStepperSpeed(maxStepperSpeed);
//  // stepper.setMaxSpeed(maxStepperSpeed);
//  // stepper.setAcceleration(1000);
//  damper.write(damperOffPos);
//}
//
//void setStepperSpeed(int speed) {
//  // stepper.setMaxSpeed(maxStepperSpeed);
//  stepper.setSpeed(speed);
//  //stepper.setMaxSpeed(maxStepperSpeed);
//  //stepper.setAcceleration(1000);
//}
//
//void noteOnHandler(byte channel, byte pitch, byte velocity) {
//  Serial.println("running noteOnHandler");
//  Serial.print("channel: ");
//  Serial.println(channel);
//  Serial.print("pitch: ");
//  Serial.println(pitch);
//  Serial.print("velocity: ");
//  Serial.println(velocity);
//
//  if(velocity > 0) {
//    playNote();
//  }
//}
//
//void noteOffHandler(byte channel, byte pitch, byte velocity) {
//  Serial.println("running noteOffHandler");
//}
//
//void playNote() {
//  //Serial.println("running playNote");
//  stepper.moveTo(noteSteps);
//  playingNote = true;
//  numberNotesPlayed += 1;
//  Serial.print(numberNotesPlayed);
//  Serial.println(" notes played");
//
//  // moveTo() also recalculates the speed for the next step, so need to set speed after calling it
//  setStepperSpeed(maxStepperSpeed);
//}
//
//void loop() {
//  if(analogRead(A10) > 1000 && !playingNote) {
//    playingNote = true;
//    stepper.moveTo(totalSteps);
//    stepper.setMaxSpeed(maxStepperSpeed);
//    stepper.setSpeed(maxStepperSpeed);
//  }
//
//  if(stepper.distanceToGo() == 0 && playingNote) {
//    stepper.setCurrentPosition(0);
//    playingNote = false;
//    delay(50);
//  }
//  stepper.runSpeedToPosition();
//}

