
/* motor connectors:
1a: black
2a: green
1b: red
2b:blue
*/

// using stepper motor driver 1

//const int StpENA = 48; 
//const int StpMS1A = 49; 
//const int StpMS2A = 50;
//const int StpMS3A = 51;
//const int StpSTPA = 52;
//const int StpDIRA = 53;

// using stepper motor driver 2

const int StpENB = 13; 
const int StpMS1B = 12; 
const int StpMS2B = 11;
const int StpMS3B = 10;
const int StpSTPB = 9;
const int StpDIRB = 8;

void setup() {
//  pinMode(StpMS1A, OUTPUT);
//  pinMode(StpMS2A, OUTPUT);
//  pinMode(StpMS3A, OUTPUT);
//  pinMode(StpENA, OUTPUT);
//  pinMode(StpSTPA, OUTPUT);
//  pinMode(StpDIRA, OUTPUT);
//  
  pinMode(StpMS1B, OUTPUT);
  pinMode(StpMS2B, OUTPUT);
  pinMode(StpMS3B, OUTPUT);
  pinMode(StpENB, OUTPUT);
  pinMode(StpSTPB, OUTPUT);
  pinMode(StpDIRB, OUTPUT);
  
  //set microstep resolution
//  digitalWrite(StpMS1A, LOW);
//  digitalWrite(StpMS2A, LOW);
//  digitalWrite(StpMS3A, LOW);
  
  digitalWrite(StpMS1B, LOW);
  digitalWrite(StpMS2B, LOW);
  digitalWrite(StpMS3B, LOW);
  
//  digitalWrite(StpDIRA, HIGH);
  digitalWrite(StpDIRB, HIGH); // set initial direction

//  digitalWrite(StpENA, LOW);
  digitalWrite(StpENB, LOW);

  Serial.begin(9600);
}

void loop() {
//  digitalWrite(StpSTPA, HIGH);
  for(int i = 0; i < 200; i++) {
    digitalWrite(StpSTPB, HIGH);
    delay(1);
    digitalWrite(StpSTPB, LOW);
    delay(1);
  }

  delay(1000);
  Serial.println("finished 50 turns - toggling direction");
  digitalWrite(StpDIRB, LOW);
  delay(1);
  
  for(int i = 0; i < 200; i++) {
    digitalWrite(StpSTPB, HIGH);
    delay(1);
    digitalWrite(StpSTPB, LOW);
    delay(1);
  }
  
  delay(1000);
  Serial.println("finished - toggling direction");
  digitalWrite(StpDIRB, HIGH); //toggle direction
  delay(1);
  
//  digitalWrite(StpSTPA, LOW);
//  digitalWrite(StpSTPB, LOW);
//  delay(1000);
}
