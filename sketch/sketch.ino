#include <NewPing.h>
#include "stats.h"

//TODO: Adjust Pins
#define TRIG_PIN_1 53
#define TRIG_PIN_2 47
#define TRIG_PIN_3 45
#define TRIG_PIN_4 28
#define TRIG_PIN_5 30
#define TRIG_PIN_6 32

#define ECHO_PIN_1 51
#define ECHO_PIN_2 49
#define ECHO_PIN_3 43
#define ECHO_PIN_4 69
#define ECHO_PIN_5 69
#define ECHO_PIN_6 69 

//Motor 1-4 outputs
#define MOTOR_FL_FORW 7
#define MOTOR_FL_BACKW 6
#define MOTOR_FR_FORW 9
#define MOTOR_FR_BACKW 8
#define MOTOR_BL_FORW 3
#define MOTOR_BL_BACKW 2
#define MOTOR_BR_FORW 4
#define MOTOR_BR_BACKW 1

//Ultrasonic Sensor Configuration
#define MAX_DISTANCE 400
#define CRITICAL_DISTANCE_FRONT 10
#define CRITICAL_DISTANCE_BACK 10
#define CRITICAL_DISTANCE_LEFT 5
#define CRITICAL_DISTANCE_RIGHT 5

//Color Sensor TCS32000 Configuration
#define TCS_S0 22
#define TCS_S1 24
#define TCS_S2 26
#define TCS_S3 28
#define TCS_sensor_out 30

//Acceleration Configuration
#define ACCELERATION_MAX_ITER 20
#define ACCELERATION_PAUSE 200

//Cornerung Configuration
#define CORNER_SPEED_OUTSIDE 128
#define CORNER_SPEED_INSIDE 64
#define CORNER_MAX_ITER 50

//Distances
int distance_sensor_1;
int distance_sensor_2;
int distance_sensor_3;
int distance_sensor_4;
int distance_sensor_5;
int distance_sensor_6;

//Stats
Driving_State driving_state = standing;
Object_State object_State = nothing;
Line_State line_State = not_recognized;
Colors tcs_color = color_NONE;

//TODO: Check if you can define multiple sensors with same trigger pin
NewPing sensor_1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor_2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor_3(TRIG_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sensor_4(TRIG_PIN_4, ECHO_PIN_4, MAX_DISTANCE);
NewPing sensor_5(TRIG_PIN_5, ECHO_PIN_5, MAX_DISTANCE);
NewPing sensor_6(TRIG_PIN_6, ECHO_PIN_6, MAX_DISTANCE);

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
void distanceMeasurement();

//Driving Methods
void initAcceleration();
void forward();
void stand();
void leftRotation_90();
void rightRotation_90();
void initLeftCorner();
void initRightCorner();
void resetCorner();

//Color Sensor Methods
void readColor();
int readReadFreq();
int readGreenFreq();
int readBlueFreq();

int accelerating_speed = 255;
int accelerate_counter = 0;
int driving_speed = 128;

int corner_counter = 0;

void setup() {
  setMotorPinModes();

  driving_state = standing;
  object_State = nothing;
  line_State = not_recognized;

  Serial.begin(9600);
}

void loop() {
  distanceMeasurement();

  /*
  if(distance_sensor_1 < 10 && driving_speed != standing) {
    Serial.println("Stop");
    //stand();
    driving_state = standing;
  }
  else if(distance_sensor_1 >= 10 && driving_state == standing) {
    Serial.println("Start");
    //initAcceleration();
    driving_state = accelerating;
  }
  */

  
  //TODO: temp code for color sensor
  readColor();
  switch (tcs_color)
  {
    case color_red: Serial.println("Red"); break;
    case color_orange: Serial.println("Orange"); break;
    case color_green: Serial.println("Green"); break;
    case color_yellow: Serial.println("Yellow"); break;
    case color_brown: Serial.println("Brown"); break;
    case color_blue: Serial.println("Blue"); break;
    case color_NONE: Serial.println("NONE"); break;
    default:
      Serial.print("FAIL");
  }

  /*
  //Increase counter in case the car is cornering
  if(driving_state == left_cornering || driving_state == right_cornering) {
    corner_counter ++;
  }
  */
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
  pinMode(MOTOR_FL_BACKW, OUTPUT);
  pinMode(MOTOR_FR_FORW, OUTPUT);
  pinMode(MOTOR_FR_BACKW, OUTPUT);
  pinMode(MOTOR_BL_FORW, OUTPUT);
  pinMode(MOTOR_BL_BACKW, OUTPUT);
  pinMode(MOTOR_BR_FORW, OUTPUT);
  pinMode(MOTOR_BR_BACKW, OUTPUT);
}

//TODO: Check if delay between measurements is required
//TODO: Check sensor meas order
void distanceMeasurement() {
  distance_sensor_1 = sensor_1.ping_cm();
  distance_sensor_2 = sensor_2.ping_cm();
  distance_sensor_3 = sensor_3.ping_cm();
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
void initAcceleration() {
  accelerate_counter = 0;
  motor_1_forw(accelerating_speed);
  motor_2_forw(accelerating_speed);
  motor_3_forw(accelerating_speed);
  motor_4_forw(accelerating_speed);
  driving_state = accelerating;
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
  analogWrite(MOTOR_FL_BACKW, driving_speed);

  analogWrite(MOTOR_FR_FORW, driving_speed);
  analogWrite(MOTOR_FR_BACKW, 0);

  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, driving_speed);

  analogWrite(MOTOR_BR_FORW, driving_speed);
  analogWrite(MOTOR_BR_BACKW, 0);

  delay(1900);

  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR_FL_BACKW, 0);

  analogWrite(MOTOR_FR_FORW, 0);
  analogWrite(MOTOR_FR_BACKW, 0);

  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, 0);

  analogWrite(MOTOR_BR_FORW, 0);
  analogWrite(MOTOR_BR_BACKW, 0);
}

void rightRotation_90() {
  analogWrite(MOTOR_FL_FORW, driving_speed);
  analogWrite(MOTOR_FL_BACKW, 0);

  analogWrite(MOTOR_FR_FORW, 0);
  analogWrite(MOTOR_FR_BACKW, driving_speed);

  analogWrite(MOTOR_BL_FORW, driving_speed);
  analogWrite(MOTOR_BL_BACKW, 0);

  analogWrite(MOTOR_BR_FORW, 0);
  analogWrite(MOTOR_BR_BACKW, driving_speed);

  delay(1900);

  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR_FL_BACKW, 0);

  analogWrite(MOTOR_FR_FORW, 0);
  analogWrite(MOTOR_FR_BACKW, 0);

  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, 0);

  analogWrite(MOTOR_BR_FORW, 0);
  analogWrite(MOTOR_BR_BACKW, 0);
}

void initLeftCorner() {
  motor_1_forw(CORNER_SPEED_INSIDE);
  motor_2_forw(CORNER_SPEED_OUTSIDE);
  motor_3_forw(CORNER_SPEED_INSIDE);
  motor_4_forw(CORNER_SPEED_OUTSIDE);

  corner_counter = 0;
  driving_state = left_cornering;
}

void initRightCorner() {
  motor_1_forw(CORNER_SPEED_OUTSIDE);
  motor_2_forw(CORNER_SPEED_INSIDE);
  motor_3_forw(CORNER_SPEED_OUTSIDE);
  motor_4_forw(CORNER_SPEED_INSIDE);

  corner_counter = 0;
  driving_state = right_cornering;
}

void forward() {
  motor_1_forw(driving_speed);
  motor_2_forw(driving_speed);
  motor_3_forw(driving_speed);
  motor_4_forw(driving_speed);

  driving_state = straight;
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
