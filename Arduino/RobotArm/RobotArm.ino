#include <Servo.h>
#include <IRremote.h>

/** IR Constants **/
#define IRRepeat 0
#define IRLeft 3141861120
#define IRRight 3158572800
#define IRStop 3208707840
#define IRPin 3

/** Motor Constants **/
#define motorSpeedPin 5
#define motorDir1Pin 4
#define motorDir2Pin 2
#define motorSpeed 150

IRrecv irrecv(IRPin);

/** All movements the robot is capable of **/
enum controls{
  NONE,
  LEFT,
  RIGHT
};
controls currentControl;

int longArmPin = 9;
int clawPin = 12;

Servo longArmServo;
Servo clawServo;

int longArmPositionGround = 1000;
int longArmPositionUp = 1500;
int clawPositionOpen = 1000;
int clawPositionClosed = 1925;

void setup() {
  
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDir1Pin, OUTPUT);
  pinMode(motorDir2Pin, OUTPUT);
  longArmServo.attach(longArmPin);
  clawServo.attach(clawPin);
  
}

void loop() {

  handleIRInput(&currentControl);
  rotate(currentControl);
  
  /*openClaw();
  goDown();
  closeClaw();
  returnUp();*/
  
}

void handleIRInput(controls *currentControl){
  
  if(irrecv.decode()){
    if(irrecv.decodedIRData.decodedRawData != IRRepeat){
      if(irrecv.decodedIRData.decodedRawData == IRLeft)
        *currentControl = LEFT;
      else if(irrecv.decodedIRData.decodedRawData == IRRight)
        *currentControl = RIGHT;
      else if(irrecv.decodedIRData.decodedRawData == IRStop)
        *currentControl = NONE;
    }

    irrecv.resume();
  }
  
}

void rotate(controls current){
  Serial.println(current);
  switch(current){
    case LEFT:
      digitalWrite(motorDir1Pin, HIGH);
      digitalWrite(motorDir2Pin, LOW);
      analogWrite(motorSpeedPin, motorSpeed);
      break;
    case RIGHT:
      digitalWrite(motorDir1Pin, LOW);
      digitalWrite(motorDir2Pin, HIGH);
      analogWrite(motorSpeedPin, motorSpeed);
      break;
    default:
      analogWrite(motorSpeedPin, 0);
      break;
  }
  
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
