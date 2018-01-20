#include <Servo.h>

Servo damper;  // create servo object to control the damper

int pos = 0;
int positions[] = {0, 45, 90, 135, 180};

void setup() {
  Serial.begin(9600);
  damper.attach(21);  // attaches the damper on pin 21 to the servo object
}


void loop() {
  for (int i = 0; i < 5; i++) {
    damper.write(positions[i]);
    delay(2000);
    Serial.print("current: ");
    Serial.println(damper.read());
    Serial.print("set: ");
    Serial.println(positions[i]);
  }

  for (int i = 4; i >= 0; i--) {
    damper.write(positions[i]);
    delay(2000);
    Serial.print("current: ");
    Serial.println(damper.read());
    Serial.print("set: ");
    Serial.println(positions[i]);
  }
}

