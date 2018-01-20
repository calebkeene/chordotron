#include <Servo.h>

Servo damper;  // create servo object to control the damper

int pos = 0;
int positions[] = {0, 45, 90, 135, 180};

const int change = 30;

void setup() {
  Serial.begin(9600);
  damper.attach(21);  // attaches the damper on pin 21 to the servo object
  damper.write(50);
  pos = 95;
}


void loop() {
  damper.write(pos);
  Serial.println("first");
  delay(5000);
  damper.write(pos+change);
  Serial.println("second"); 
  delay(5000);
}


