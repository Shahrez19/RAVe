#include <AFMotor.h>
AF_DCMotor motor1(3);

void setup() {
  motor1.setSpeed(250);
  Serial.begin(115200);
}

void loop() {
    motor1.run(FORWARD);
    delay(1500);
    motor1.run(RELEASE);
    delay(100);
    exit(0);
//  }
}
