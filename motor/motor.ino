#include <AFMotor.h>
AF_DCMotor motor1(3);

void setup(){
  motor1.setSpeed(250);
}

void loop(){
  if(Serial.available()>0){
  command = Serial.read();
  }
  else{
    reset();
  }
  send_command(command,time);
}

// Turn off all motors
void reset(){
motor1.Release;
}
void forward(){
motor1.run(FORWARD);
}

