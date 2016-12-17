#include <AFMotor.h>
AF_DCMotor motor1(3);
AF_DCMotor motor2(1);

//Control Flags
int flag = 0;
int command = 55;
// Turn off all motors
void reset(){
Serial.println("RESET!!!");
motor1.run(RELEASE);
motor2.run(RELEASE);
}

void forward(){
motor1.run(FORWARD);
Serial.println("GOING FORWARD In ARDUINO");
delay(3000);
motor1.run(RELEASE);
delay(200); //Coast for 1/10 second to prevent power pushback
}

void CommandCenter(int command){
	switch (command) {

	    // Release all Motors
	    case 0: reset(); break;

	    //Simple Commands
	    case 1: forward(); break;

	    default: Serial.print("\nINVALID ORDER!... TURNING OFF!\n");
	}
}

void setup(){
  motor1.setSpeed(250);
  motor2.setSpeed(200);

  Serial.begin(115200);
  Serial.print("\n\n Motors Started......\n");
}

void loop(){

  reset();

  //GET input
  if(Serial.available() > 0){
  	command = Serial.read() - 65;
  	Serial.print("I received: ");
  	Serial.println(command);
  	flag = 1;
  }

  if(flag){
  	// execute Command
    CommandCenter(command);
  }
}
