#include <Wire.h>
#include <AFMotor.h>

AF_DCMotor motor1(3); // main drive
AF_DCMotor motor2(1); // steering servo

#define SLAVE_ADDRESS SDA
#define FWD 0
#define BWD 1
#define LFT 3
#define RGT 4

int number = -1;

void setup() {
  // motor setup
  motor1.setSpeed(250);

  // serial setup
  pinMode (13, OUTPUT);
  Serial.begin(9600);
  
  // init i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // i2c callbacks
  Wire.onReceive(receiveData);
  Wire.onRequest (sendData);

  Serial.println("READY FOR INPUT");
}

void loop() {
  delay(1); // just wait for i2c iput
}

void receiveData(int byteCount) {
  while (Wire.available()) {
    number = Wire.read();
    Serial.print("data received: ");
    Serial.println(number);
      switch (number) {
      case 1:
        Serial.println("FORWARD!");
        motor1.run(FORWARD);
        delay(250000);
        motor1.run(RELEASE);
        break;
      case 2:
        motor1.run(BACKWARD);
        delay(250000);
        motor1.run(RELEASE);
        break;
      case 3:
        motor1.run(FORWARD);
        motor2.run(FORWARD);
        delay(500000);
        motor1.run(RELEASE);
        motor2.run(RELEASE);
      case 4:
        motor1.run(FORWARD);
        motor2.run(BACKWARD);
        delay(500000);
        motor1.run(RELEASE);
        motor2.run(RELEASE);
      default: 
        continue;
      break;
    }
  }
}

void sendData() {
  Wire.write(number);
}

