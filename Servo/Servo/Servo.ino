#include <ESP32Servo.h>

static const int servoPin = 27;

Servo servo1;

void setup() {

  Serial.begin(9600);
  servo1.attach(servoPin);
}

void loop() {
  // for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
  //   servo1.write(posDegrees);
  //   Serial.println(posDegrees);
  //   delay(20);
  // }
  // for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
  //   servo1.write(posDegrees);
  //   Serial.println(posDegrees);
  //   delay(20);
  // }
  servo1.write(180);
  Serial.println("180");
  delay(2000);
  servo1.write(0);
  Serial.println("0");
  delay(2000);
}