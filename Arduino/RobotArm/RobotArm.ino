#include <Servo.h>
#include <IRremote.h>

/** IR Constants **/
#define IRRepeat 0
#define IRLeft 3141861120
#define IRRight 3158572800
#define IRStop 3208707840
#define IRUp 3108437760
#define IRDown 3927310080
#define IRClose 4161273600
#define IROpen 4127850240
#define IRPin 3

/** Motor Constants **/
#define motorSpeedPin 5
#define motorDir1Pin 4
#define motorDir2Pin 2
#define motorSpeed 200

/** Arm and Claw Constants **/
#define armPin 9
#define clawPin 10
#define armPositionDown 1000
#define armPositionUp 1500
#define clawPositionOpen 1000
#define clawPositionClosed 1925

/** All movements the robot is capable of **/
enum controls{
  NONE,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  OPEN,
  CLOSE
};

IRrecv irrecv(IRPin);
Servo armServo;
Servo clawServo;

controls currentControl;
int currentArmPosition;
int currentClawPosition;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDir1Pin, OUTPUT);
  pinMode(motorDir2Pin, OUTPUT);
  armServo.attach(armPin);
  clawServo.attach(clawPin);

  //set initial positions
  returnUp();
  closeClaw();
}

void loop() {

  handleIRInput(&currentControl);
  rotate(currentControl);
  moveArm(currentControl);
  openCloseClaw(currentControl);
   
}

void handleIRInput(controls *current){
  if(irrecv.decode()){
    if(irrecv.decodedIRData.decodedRawData != IRRepeat){
      switch(irrecv.decodedIRData.decodedRawData){
        case IRLeft:
          *current = LEFT;
          break;
        case IRRight:
          *current = RIGHT;
          break;
        case IRStop:
          *current = NONE;
          break;
        case IRUp:
          *current = UP;
          break;
        case IRDown:
          *current = DOWN;
          break;
        case IROpen:
          *current = OPEN;
          break;
        case IRClose:
          *current = CLOSE;
          break;
      }
    }

    irrecv.resume();
  }
}

void rotate(controls current){
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

void moveArm(controls current){
  switch(current){
    case UP:
      addArmIncrement(currentArmPosition + 1, 1);
      break;
    case DOWN:
      addArmIncrement(currentArmPosition - 1, 5);
      break;
    default:
      
      break;
  }
}

void openCloseClaw(controls current){
  switch(current){
    case OPEN:
      addClawIncrement(currentClawPosition - 1);
      break;
    case CLOSE:
      addClawIncrement(currentClawPosition + 1);
      break;
    default:
      
      break;
  }
}

void addArmIncrement(int increment, int delayTime){
   if(increment > armPositionUp || increment < armPositionDown)
    return;
      
   armServo.writeMicroseconds(increment);
   currentArmPosition = increment;
   delay(delayTime);
}

void goDown(){
  for(int i = armPositionUp; i > armPositionDown; i--){
    addArmIncrement(i, 5);
  }
}

void returnUp(){
  if(currentArmPosition == 0)
    currentArmPosition = armPositionDown;
  
  for(int i = armPositionDown; i < armPositionUp; i++){
    addArmIncrement(i, 1);
  }
}

void addClawIncrement(int increment){
   if(increment > clawPositionClosed || increment < clawPositionOpen)
    return;
      
   clawServo.writeMicroseconds(increment);
   currentClawPosition = increment;
   delay(5);
}

void openClaw(){
  for(int i = currentClawPosition; i > clawPositionOpen; i--){
    addClawIncrement(i);
  }
}

void closeClaw(){
  if(currentClawPosition == 0)
    currentClawPosition = clawPositionOpen;
    
  for(int i = currentClawPosition; i < clawPositionClosed; i++){
    addClawIncrement(i);
  }
}
