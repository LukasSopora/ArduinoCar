#include <NewPing.h>
#include "stats.h"

//TODO: Adjust Pins
#define TRIG_PIN 9
#define ECHO_PIN_1 8
#define ECHO_PIN_2 3
#define ECHO_PIN_3 4
#define ECHO_PIN_4 5
#define ECHO_PIN_5 6
#define ECHO_PIN_6 7

//Motor 1-4 outputs
#define MOTOR1_FORW 10
#define MOTOR1_BACKW 11
#define MOTOR2_FORW 12
#define MOTOR2_BACKW 13
#define MOTOR3_FORW 4
#define MOTOR3_BACKW 5
#define MOTOR4_FORW 3
#define MOTOR4_BACKW 2

int speed = 128; 

//Ultrasonic Sensor max distance is 400cm
#define MAX_DISTANCE 400

//Distances
unsigned int distance_sensor_1;
unsigned int distance_sensor_2;
unsigned int distance_sensor_3;
unsigned int distance_sensor_4;
unsigned int distance_sensor_5;
unsigned int distance_sensor_6;

//Stats
Driving_State driving_state = standstill;
Object_State object_State = nothing;
Line_State line_State = not_recognized;

//TODO: Check if you can define multiple sensors with same trigger pin
NewPing sensor_1(TRIG_PIN, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor_2(TRIG_PIN, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor_3(TRIG_PIN, ECHO_PIN_3, MAX_DISTANCE);
NewPing sensor_4(TRIG_PIN, ECHO_PIN_4, MAX_DISTANCE);
NewPing sensor_5(TRIG_PIN, ECHO_PIN_5, MAX_DISTANCE);
NewPing sensor_6(TRIG_PIN, ECHO_PIN_6, MAX_DISTANCE);

void print_state();
void updateStates();
void setMotorPinModes();
void accelerate();
void stand();
void distance_measurement();
void leftTurn();
void rightTurn();

bool isStop;


void setup() {
  setMotorPinModes();

  isStop = true;

  Serial.begin(9600);
  
  analogWrite(MOTOR4_FORW, speed);
  analogWrite(MOTOR3_FORW, speed);
  analogWrite(MOTOR2_FORW, speed);
  analogWrite(MOTOR1_FORW, speed);
  delay(1000);

  analogWrite(MOTOR1_FORW, 0);
  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR4_FORW, 0);

  delay(1000);
  analogWrite(MOTOR4_BACKW, speed);
  analogWrite(MOTOR3_BACKW, speed);
  analogWrite(MOTOR2_BACKW, speed);
  analogWrite(MOTOR1_BACKW, speed);
  delay(1000);

  analogWrite(MOTOR1_BACKW, 0);
  analogWrite(MOTOR2_BACKW, 0);
  analogWrite(MOTOR3_BACKW, 0);
  analogWrite(MOTOR4_BACKW, 0);
}

void loop() {
  distance_measurement();

  if(distance_sensor_1 < 10 && !isStop) {
    Serial.println("Stop");
    //stand();
    isStop = true;
  }
  else if(distance_sensor_1 >= 10 && isStop) {
    Serial.println("Start");
    //accelerate();
    isStop = false;
  }
}

void leftTurn() {
  analogWrite(MOTOR1_FORW, 0);
  analogWrite(MOTOR1_BACKW, speed);

  analogWrite(MOTOR2_FORW, speed);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, speed);

  analogWrite(MOTOR4_FORW, speed);
  analogWrite(MOTOR4_BACKW, 0);

  delay(1900);

  analogWrite(MOTOR1_FORW, 0);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, 0);
}

void rightTurn() {
  analogWrite(MOTOR1_FORW, speed);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, speed);

  analogWrite(MOTOR3_FORW, speed);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, speed);

  delay(1900);

  analogWrite(MOTOR1_FORW, 0);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, 0);
}

void print_state() {
  Serial.print("Driving State: ");
  Serial.println(driving_state);

  Serial.print("Object State: ");
  Serial.println(object_State);

  Serial.print("Line State: ");
  Serial.println(line_State);

  //TODO: implement print-format function
  Serial.print("Distances: ");
  Serial.print(distance_sensor_1);
  Serial.print(" - ");
  Serial.print(distance_sensor_2);
  Serial.print(" - ");
  Serial.print(distance_sensor_3);
  Serial.print(" - ");
  Serial.print(distance_sensor_4);
  Serial.print(" - ");
  Serial.print(distance_sensor_5);
  Serial.print(" - ");
  Serial.print(distance_sensor_6);
}

void updateStates() {
  //TODO: implementation
  if(distance_sensor_1 < 10) {
    object_State = front;
  }
  else {
    object_State = nothing;
  }
}

void setMotorPinModes() {
  pinMode(MOTOR1_FORW, OUTPUT);
  pinMode(MOTOR1_BACKW, OUTPUT);
  pinMode(MOTOR2_FORW, OUTPUT);
  pinMode(MOTOR2_BACKW, OUTPUT);
  pinMode(MOTOR3_FORW, OUTPUT);
  pinMode(MOTOR3_BACKW, OUTPUT);
  pinMode(MOTOR4_FORW, OUTPUT);
  pinMode(MOTOR4_BACKW, OUTPUT);
}

void accelerate() {
  analogWrite(MOTOR1_FORW, speed);
  analogWrite(MOTOR2_FORW, speed);
  analogWrite(MOTOR3_FORW, speed);
  analogWrite(MOTOR4_FORW, speed);
  
  driving_state = strait;
}

void stand() {
  analogWrite(MOTOR1_FORW, 0);
  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR4_FORW, 0);
  
  driving_state = strait;
}

//TODO: Check if delay between measurements is required
//TODO: Check sensor meas order
void distance_measurement() {
  distance_sensor_1 = sensor_1.ping_cm();
  //distance_sensor_2 = sensor_2.ping_cm();
  //distance_sensor_3 = sensor_3.ping_cm();
  //distance_sensor_4 = sensor_4.ping_cm();
  //distance_sensor_5 = sensor_5.ping_cm();
  //distance_sensor_6 = sensor_6.ping_cm();
}
