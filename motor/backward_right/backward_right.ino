#include <AFMotor.h>
AF_DCMotor motor1(3);
AF_DCMotor motor2(1);

void setup() {
  motor1.setSpeed(250);
  motor2.setSpeed(250);
  Serial.begin(115200);
}

void loop() {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    delay(3000);
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    delay(100);
    exit(0);
}
