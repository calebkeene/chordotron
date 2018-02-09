/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo1;
Servo myservo2;// create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 60;    // variable to store the servo position

void setup() {
  myservo1.attach(20);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(40);
}

void loop() {
//  for (pos = 0; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(1);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 120; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo2.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(1);                       // waits 15ms for the servo to reach the position
//  }

  myservo1.write(10);
  delay(1000);
  myservo1.write(110);
}

