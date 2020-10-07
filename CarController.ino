#include "Servo.h"
#include "ArduinoJson.h"

//Pin declarations
int MOTOR1_DIRECTION_PIN = 10;
int MOTOR1_SPEED_PIN = 12;
int MOTOR2_DIRECTION_PIN =11;
int MOTOR2_SPEED_PIN = 13;
int STEERING_SERVO_PIN = 9;

//Steering constants
int STEERING_CENTER_POSITION = 135;
int STEERING_MAX_RIGHT = 45;
int STEERING_MAX_LEFT = 180;

//JSON Constants
const int JSON_DOCUMENT_SIZE = 56;

//Globals
Servo _steeringServo;
StaticJsonDocument<JSON_DOCUMENT_SIZE> _jsonDocument;

void setup()
{
  Serial.begin(9600);
  
  //Setup motor direction pins for writing
  pinMode(MOTOR1_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR2_DIRECTION_PIN, OUTPUT);

  //Attach servo pin
  _steeringServo.attach(STEERING_SERVO_PIN);
  
  resetSteeringToInitialPosition();

  Serial.setTimeout(50);

  
  doSteeringCalibrationTest();
  driveMotors(1, HIGH);
  delay(1000);
  driveMotors(0, LOW);
  delay(1000);
  driveMotors(-1, HIGH);
  delay(1000);
  driveMotors(0, LOW);
  
}

void loop()
{     
  while(Serial.available()){         
    String jsonString = Serial.readString();    
    Serial.println();
    Serial.print("[");
    Serial.print(jsonString);
    Serial.print("]");
    char json[jsonString.length()];
    jsonString.toCharArray(json, jsonString.length() + 1);
    processJson(json);        
  }

  while (!Serial.available()){
    delay(50);  
  }
  
}

//Value 
void driveMotors(int speed, int direction)
{
    digitalWrite(MOTOR1_DIRECTION_PIN,direction);
    digitalWrite(MOTOR2_DIRECTION_PIN, direction);
    analogWrite(MOTOR1_SPEED_PIN, speed);
    analogWrite(MOTOR2_SPEED_PIN, speed);      
}

void resetSteeringToInitialPosition(){
  _steeringServo.write(STEERING_CENTER_POSITION);
}

void steer(int value){
  _steeringServo.write(value);
}

void doSteeringCalibrationTest(){
  resetSteeringToInitialPosition();
  delay(500);
  steer(STEERING_MAX_RIGHT);
  delay(500);
  steer(STEERING_MAX_LEFT);
  delay(500);
  resetSteeringToInitialPosition();
}

void processJson(char json[]){    
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(_jsonDocument, json);    

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());      
    }    
    else{
      int driveValue = _jsonDocument["drive"];
      int steerValue = _jsonDocument["steer"];
  
      performInstruction(driveValue, steerValue);  
    }    
}

void performInstruction(int driveValue, int steerValue){
  Serial.println("");
  Serial.print("Instruction Received drive value [");
  Serial.print(driveValue);
  Serial.print("] steer value [");
  Serial.print(steerValue);
  Serial.print("]");

  int direction = HIGH;
  if(driveValue < 0){
      direction = LOW;
  }

  //move motors
  driveMotors(driveValue, direction);
  //move servo
  steer(steerValue);
}

void performInstruction(const char* instruction, int value){
  Serial.println("");
  Serial.print("Instruction Received [");
  Serial.print(instruction);
  Serial.print("] Value [");
  Serial.print(value);
  Serial.print("]");
  
  if(strcmp(instruction, "drive") == 0){    
    int direction = HIGH;
    if(value < 0){
      direction = LOW;
    }

    driveMotors(value, direction);
  }
  else if(strcmp(instruction, "steer") == 0){
    steer(value);
  }
  else if(strcmp(instruction, "steerCalibrate") == 0){
    doSteeringCalibrationTest();
  }
}
