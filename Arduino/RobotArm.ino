#include <Servo.h>

int longArmPin = 9;
int clawPin = 12;

Servo longArmServo;
Servo clawServo;

int longArmPositionGround = 1000;
int longArmPositionUp = 1500;
int clawPositionOpen = 1000;
int clawPositionClosed = 1925;

void setup() {
  longArmServo.attach(longArmPin);
  clawServo.attach(clawPin);
}

void loop() {
  
  openClaw();
  goDown();
  closeClaw();
  returnUp();
  
}

void goDown(){
  for(int i = longArmPositionUp; i > longArmPositionGround; i--){
    longArmServo.writeMicroseconds(i);
    delay(5);
  }
}

void returnUp(){
  for(int i = longArmPositionGround; i < longArmPositionUp; i++){
    longArmServo.writeMicroseconds(i);
    delay(1);
  }
}

void openClaw(){
  for(int i = clawPositionClosed; i > clawPositionOpen; i--){
    clawServo.writeMicroseconds(i);
    delay(5);
  }
}

void closeClaw(){
  for(int i = clawPositionOpen; i < clawPositionClosed; i++){
    clawServo.writeMicroseconds(i);
    delay(5);
  }
}
