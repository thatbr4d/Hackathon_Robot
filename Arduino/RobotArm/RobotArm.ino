#include <Servo.h>
#include <IRremote.h>
#include <Ethernet.h>
#include "ThingSpeak.h"

/** IR Constants **/
#define IRRepeat 0
#define IRLeft 3141861120
#define IRRight 3158572800
#define IRStop 3208707840
#define IRUp 3108437760
#define IRDown 3927310080
#define IRClose 4161273600
#define IROpen 4127850240
#define IRWave 4077715200
#define IRParade 3877175040
#define IRCrazy 2707357440
#define IRPin 3

/** Motor Constants **/
#define motorSpeedPin 5
#define motorDir1Pin 4
#define motorDir2Pin 2
#define motorSpeed 250

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
int soundVal;
int desiredSoundPos;

/** Ethernet and Thingspeak IoT **/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);
EthernetClient client;
unsigned long myChannelNumber = 1448050;
const char * myWriteAPIKey = "H7J3KD5O031GVN65";

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDir1Pin, OUTPUT);
  pinMode(motorDir2Pin, OUTPUT);
  armServo.attach(armPin);
  clawServo.attach(clawPin);

  InitializeEthernetAndThingSpeak();
  
  //set initial positions
  returnUp();
  closeClaw();
}

void loop() {
  
  handleSerialInput();
  handleIRInput();  
  rotate(currentControl);
  moveArm(currentControl);
  openCloseClaw(currentControl);
   
}

void handleIRInput(){
  if(irrecv.decode()){
    if(irrecv.decodedIRData.decodedRawData != IRRepeat){
      Serial.println(irrecv.decodedIRData.decodedRawData);
      switch(irrecv.decodedIRData.decodedRawData){
        case IRLeft:
          currentControl = LEFT;
          SendTelemetryToThingSpeak("Rotate Left", "Infrared Communication");
          break;
        case IRRight:
          currentControl = RIGHT;
          SendTelemetryToThingSpeak("Rotate Right", "Infrared Communication");
          break;
        case IRStop:
          currentControl = NONE;
          SendTelemetryToThingSpeak("Pause", "Infrared Communication");
          break;
        case IRUp:
          currentControl = UP;
          SendTelemetryToThingSpeak("Arm Up", "Infrared Communication");
          break;
        case IRDown:
          currentControl = DOWN;
          SendTelemetryToThingSpeak("Arm Down", "Infrared Communication");
          break;
        case IROpen:
          currentControl = OPEN;          
          SendTelemetryToThingSpeak("Claw Open", "Infrared Communication");
          break;
        case IRClose:
          currentControl = CLOSE;
          SendTelemetryToThingSpeak("Claw Close", "Infrared Communication");
          break;
        case IRWave:
          wave();
          SendTelemetryToThingSpeak("Wave", "Infrared Communication");
          break;
        case IRParade:
          paradeWave();
          SendTelemetryToThingSpeak("Parade Wave", "Infrared Communication");
          break;
        case IRCrazy:
          goCrazy();
          SendTelemetryToThingSpeak("Go Crazy", "Infrared Communication");
          break;
      }
    }

    irrecv.resume();
  }
}

void handleSerialInput(){  
  if (Serial.available())  {
    switch(Serial.read()){
      case '0':
        currentControl = NONE;
        SendTelemetryToThingSpeak("Pause", "Serial Communication");
        break;
      case '1':
        currentControl = LEFT;
        SendTelemetryToThingSpeak("Rotate Left", "Serial Communication");
        break;
      case '2':
        currentControl = RIGHT;
        SendTelemetryToThingSpeak("Rotate Right", "Serial Communication");
        break;
      case '3':
        currentControl = UP;
        SendTelemetryToThingSpeak("Arm Up", "Serial Communication");
        break;
      case '4':
        currentControl = DOWN;
        SendTelemetryToThingSpeak("Arm Down", "Serial Communication");
        break;
      case '5':
        currentControl = OPEN;
        SendTelemetryToThingSpeak("Claw Open", "Serial Communication");
        break;
      case '6':
        currentControl = CLOSE;
        SendTelemetryToThingSpeak("Close Close", "Serial Communication");
        break;
    }
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
      addArmIncrement(currentArmPosition + 1, 3);
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

void wave(){
  for(int numWaves = 0; numWaves < 3; numWaves++){
    for(int i = currentClawPosition; i > 1400; i--){
        clawServo.writeMicroseconds(i);
        currentClawPosition = i;
        delay(1);
    }
    for(int i = currentClawPosition; i < 1800; i++){
        clawServo.writeMicroseconds(i);
        currentClawPosition = i;
        delay(1);
    }
  }
}

void paradeWave(){
  clawServo.writeMicroseconds(1300);
  currentClawPosition = 1300;

  for(int numWaves = 0; numWaves < 6; numWaves++){
    rotate(LEFT);
    delay(500);
    rotate(RIGHT);
    delay(500);
  }
}

void goCrazy(){
  for(int numCrazy = 0; numCrazy < 15; numCrazy++){
    armServo.writeMicroseconds(800);
    delay(100);
    armServo.writeMicroseconds(1600);
    delay(100);
  }
}

void SendTelemetryToThingSpeak(String origin, String process){
  ThingSpeak.setField(1, origin);
  ThingSpeak.setField(2, process);
  ThingSpeak.setField(3, random(0, 100));
  ThingSpeak.setField(4, random(0, 100));

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void InitializeEthernetAndThingSpeak(){
  Ethernet.init(10);  // Most Arduino Ethernet hardware
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
      
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
