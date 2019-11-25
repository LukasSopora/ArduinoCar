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
#define MOTOR_FL_FORW 10
#define MOTOR_FL_BACKW 11
#define MOTOR_FR_FORW 12
#define MOTOR_FR_BACKW 13
#define MOTOR_BL_FORW 4
#define MOTOR_BL_BACKW 5
#define MOTOR_BR_FORW 3
#define MOTOR_BR_BACKW 2

//Ultrasonic Sensor Configuration
#define MAX_DISTANCE 400
#define CRITICAL_DISTANCE_FRONT 10
#define CRITICAL_DISTANCE_BACK 10
#define CRITICAL_DISTANCE_LEFT 5
#define CRITICAL_DISTANCE_RIGHT 5

//Color Sensor TCS32000 Configuration
#define TCS_S0 69
#define TCS_S1 69
#define TCS_S2 69
#define TCS_S3 69
#define TCS_sensor_out 69

//Acceleration Configuration
#define ACCELERATION_MAX_ITER 20
#define ACCELERATION_PAUSE 200

//Distances
unsigned int distance_sensor_1;
unsigned int distance_sensor_2;
unsigned int distance_sensor_3;
unsigned int distance_sensor_4;
unsigned int distance_sensor_5;
unsigned int distance_sensor_6;

//Stats
Driving_State driving_state = standing;
Object_State object_State = nothing;
Line_State line_State = not_recognized;
Colors tcs_color = color_NONE;

//TODO: Check if you can define multiple sensors with same trigger pin
NewPing sensor_1(TRIG_PIN, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor_2(TRIG_PIN, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor_3(TRIG_PIN, ECHO_PIN_3, MAX_DISTANCE);
NewPing sensor_4(TRIG_PIN, ECHO_PIN_4, MAX_DISTANCE);
NewPing sensor_5(TRIG_PIN, ECHO_PIN_5, MAX_DISTANCE);
NewPing sensor_6(TRIG_PIN, ECHO_PIN_6, MAX_DISTANCE);

//Motor Handling Methods
void motor_1_forw(int speed);
void motor_1_backw(int speed);
void motor_2_forw(int speed);
void motor_2_backw(int speed);
void motor_3_forw(int speed);
void motor_3_backw(int speed);
void motor_4_forw(int speed);
void motor_4_backw(int speed);

void print_states();
void updateStates();
void setMotorPinModes();
void accelerate();
void stand();
void distanceMeasurement();
void leftRotation_90();
void rightRotation_90();

//Color Sensor Methods
void readColor();
int readReadFreq();
int readGreenFreq();
int readBlueFreq();

int accelerating_speed = 255;
int accelerate_counter = 0;
int driving_speed = 128;

bool isStop;


void setup() {
  setMotorPinModes();

  driving_state = standing;
  object_State = nothing;
  line_State = not_recognized;

  isStop = true;

  Serial.begin(9600);
}

void loop() {
  distanceMeasurement();

  if(distance_sensor_1 < 10 && driving_speed != standing) {
    Serial.println("Stop");
    //stand();
    driving_state = standing;
  }
  else if(distance_sensor_1 >= 10 && driving_state == standing) {
    Serial.println("Start");
    //accelerate();
    driving_state = accelerating;
  }
}

void print_states() {
  Serial.print("Driving State: ");
  Serial.println(driving_state);

  Serial.print("Object State: ");
  Serial.println(object_State);

  Serial.print("Line State: ");
  Serial.println(line_State);

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
    object_State = front_center;
  }
  else {
    object_State = nothing;
  }
}

void setMotorPinModes() {
  pinMode(MOTOR_FL_FORW, OUTPUT);
  pinMode(MOTOR1_BACKW, OUTPUT);
  pinMode(MOTOR2_FORW, OUTPUT);
  pinMode(MOTOR2_BACKW, OUTPUT);
  pinMode(MOTOR3_FORW, OUTPUT);
  pinMode(MOTOR3_BACKW, OUTPUT);
  pinMode(MOTOR4_FORW, OUTPUT);
  pinMode(MOTOR4_BACKW, OUTPUT);
}

//TODO: Check if delay between measurements is required
//TODO: Check sensor meas order
void distanceMeasurement() {
  distance_sensor_1 = sensor_1.ping_cm();
  //distance_sensor_2 = sensor_2.ping_cm();
  //distance_sensor_3 = sensor_3.ping_cm();
  //distance_sensor_4 = sensor_4.ping_cm();
  //distance_sensor_5 = sensor_5.ping_cm();
  //distance_sensor_6 = sensor_6.ping_cm();
}

#pragma region Color Sensor Methods
//TODO: check if delay between color freq reading is required
void readColor() {
  int r = readReadFreq();
  delay(50);
  int g = readGreenFreq();
  delay(50);
  int b = readBlueFreq();


  if(r<45 & r>32 & g<65 & g>55){
    tcs_color = color_red;
  }
  if(g<55 & g>43 & b<47 &b>35){
    tcs_color = color_orange;
  }
  if(r<53 & r>40 & g<53 & g>40){
    tcs_color = color_green;
  }
  if(r<38 & r>24 & g<44 & g>30){
    tcs_color = color_yellow;
  }
  if(r<56 & r>46 & g<65 & g>55){
    tcs_color = color_brown;
  }
  if (g<58 & g>45 & b<40 &b>26){
    tcs_color = color_blue;
  }
}

int readReadFreq() {
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, LOW);
  return pulseIn(TCS_sensor_out, LOW);
}

int readGreenFreq() {
  digitalWrite(TCS_S2, HIGH);
  digitalWrite(TCS_S3, HIGH);
  return pulseIn(TCS_sensor_out, LOW);
}

int readBlueFreq() {
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, HIGH);
  return pulseIn(TCS_sensor_out, LOW);
}
#pragma endregion

#pragma region Movement Methods
void accelerate() {
  if(accelerate_counter < ACCELERATION_MAX_ITER) {
    motor_1_forw(accelerating_speed);
    motor_2_forw(accelerating_speed);
    motor_3_forw(accelerating_speed);
    motor_4_forw(accelerating_speed);
    accelerate_counter++;
  }
  else {
    motor_1_forw(driving_speed);
    motor_2_forw(driving_speed);
    motor_3_forw(driving_speed);
    motor_4_forw(driving_speed);
  }

  driving_state = straight;
}

void stand() {
  motor_1_forw(0);
  motor_2_forw(0);
  motor_3_forw(0);
  motor_4_forw(0);

  driving_state = standing;
}

void leftRotation_90() {
  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR1_BACKW, driving_speed);

  analogWrite(MOTOR2_FORW, driving_speed);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, driving_speed);

  analogWrite(MOTOR4_FORW, driving_speed);
  analogWrite(MOTOR4_BACKW, 0);

  delay(1900);

  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, 0);
}

void rightRotation_90() {
  analogWrite(MOTOR_FL_FORW, driving_speed);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, driving_speed);

  analogWrite(MOTOR3_FORW, driving_speed);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, driving_speed);

  delay(1900);

  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR1_BACKW, 0);

  analogWrite(MOTOR2_FORW, 0);
  analogWrite(MOTOR2_BACKW, 0);

  analogWrite(MOTOR3_FORW, 0);
  analogWrite(MOTOR3_BACKW, 0);

  analogWrite(MOTOR4_FORW, 0);
  analogWrite(MOTOR4_BACKW, 0);
}
#pragma endregion

#pragma region Motor Handling Methods
void motor_1_forw(int speed) {
  analogWrite(MOTOR_FL_BACKW, 0);
  analogWrite(MOTOR_FL_FORW, speed);
}

void motor_1_backw(int speed) {
  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR_FL_BACKW, speed);
}

void motor_2_forw(int speed) {
  analogWrite(MOTOR_FR_BACKW, 0);
  analogWrite(MOTOR_FR_FORW, speed);
}

void motor_2_backw(int speed) {
  analogWrite(MOTOR_FR_FORW, 0);
  analogWrite(MOTOR_FR_BACKW, speed);
}

void motor_3_forw(int speed) {
  analogWrite(MOTOR_BL_BACKW, 0);
  analogWrite(MOTOR_BL_FORW, speed);
}

void motor_3_backw(int speed) {
  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, speed);
}

void motor_4_forw(int speed) {
  analogWrite(MOTOR_BR_BACKW, 0);
  analogWrite(MOTOR_BR_FORW, speed);
}

void motor_4_backw(int speed) {
  analogWrite(MOTOR_BR_FORW, 0);
  analogWrite(MOTOR_BR_BACKW, speed);
}
#pragma endregion