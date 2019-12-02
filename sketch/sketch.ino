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
#define MOTOR_FL_FORW 28
#define MOTOR_FL_BACKW 29
#define MOTOR_FR_FORW 26
#define MOTOR_FR_BACKW 27
#define MOTOR_BL_FORW 22
#define MOTOR_BL_BACKW 23
#define MOTOR_BR_FORW 24
#define MOTOR_BR_BACKW 25

//Ultrasonic Sensor Configuration
#define MAX_DISTANCE 400
#define CRITICAL_DISTANCE_FRONT 10
#define CRITICAL_DISTANCE_BACK 10
#define CRITICAL_DISTANCE_LEFT 5
#define CRITICAL_DISTANCE_RIGHT 5

//Color Sensor TCS32000 Configuration
#define TCS_S0 13
#define TCS_S1 12
#define TCS_S2 11
#define TCS_S3 10
#define TCS_sensor_out 9

//RGB ratio for color recognizion
#define COLOR_RATIO 0.7

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
void backward();
void stand();
void leftRotation_90();
void rightRotation_90();
void initLeftCorner();
void initRightCorner();
void resetCorner();
void kurve(String richtung);
void vorbeifahren(String richtung);

//Color Sensor Methods
void readColor();
void initColorSensor();

//Color Sensor Variables
int freqRed = 0;
int freqGreen = 0;
int freqBlue = 0;

double ratioRed = 0;
double ratioBlue = 0;

//Speed Variables
int accelerating_speed = 255;
int accelerate_counter = 0;
int driving_speed = 128;

int corner_counter = 0;

void setup() {
  setMotorPinModes();

  driving_state = standing;
  object_State = nothing;
  line_State = not_recognized;

  initColorSensor();

  Serial.begin(9600);
}

void loop() {
  distanceMeasurement();
  readColor();
  if(distance_sensor_2 < 15 && driving_state != standing){
    if(distance_sensor_1 < 15 && distance_sensor_3 < 15){
      Serial.println("Stop");
      stand();
      driving_state = standing;
    }
    else if(distance_sensor_1 < 15){
      Serial.println("rechts vorbeifahren");
      vorbeifahren("rechts");
    }
    else if(distance_sensor_3 < 15){
      Serial.println("links vorbeifahren");
      vorbeifahren("links");
    }
  }
  else if(distance_sensor_2 >= 15 && driving_state != straight) {
    Serial.println("Start");
    forward();
    driving_state = straight;
  }
  switch (tcs_color)
  {
  case color_red: Serial.println("Red, Rechtskurve"); kurve("rechts"); break;
  case color_blue: Serial.println("Blue, Linkskurve"); kurve("links"); break;  
  default: Serial.println("Fail"); break;
  }
  //print_states();
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
void initColorSensor() {
  pinMode(TCS_S0, OUTPUT);
  pinMode(TCS_S1, OUTPUT);
  pinMode(TCS_S2, OUTPUT);
  pinMode(TCS_S3, OUTPUT);
  pinMode(TCS_sensor_out, INPUT);

  //Set Output Frequency Scaling to 2%
  digitalWrite(TCS_S0, LOW);
  digitalWrite(TCS_S1, HIGH);
}

void readColor() {
  //Read red freq
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, LOW);
  freqRed = pulseIn(TCS_sensor_out, LOW);

  //Read green freq
  digitalWrite(TCS_S2, HIGH);
  digitalWrite(TCS_S3, HIGH);
  freqGreen = pulseIn(TCS_sensor_out, LOW);

  //Read blue freq
  digitalWrite(TCS_S2, LOW);
  digitalWrite(TCS_S3, HIGH);
  freqBlue = pulseIn(TCS_sensor_out, LOW);

  ratioRed = (double) freqRed / ((freqGreen + freqBlue) / 2);

  if(ratioRed < COLOR_RATIO) {
    tcs_color = color_red;
  }
  else {
    ratioBlue = (double) freqBlue / ((freqRed + freqGreen) / 2);

    if(ratioBlue < COLOR_RATIO) {
      tcs_color = color_blue;
    }
    else {
      tcs_color = color_NONE;
    }
  }
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

void backward(){
  motor_1_backw(driving_speed);
  motor_2_backw(driving_speed);
  motor_3_backw(driving_speed);
  motor_4_backw(driving_speed);
}

void kurve(String richtung){
  stand();
  delay(100);
  backward();
  delay(500);
  stand();
  delay(100);
  if(richtung == "rechts"){
    rightRotation_90();
  }
  else if(richtung == "links"){
    leftRotation_90();
  }
  delay(100);
}

void vorbeifahren(String richtung){
  stand();
  delay(100);
  if(richtung == "rechts"){
    rightRotation_90();
    delay(100);
    int counter = 0;
    distanceMeasurement();
    forward();
    while(distance_sensor_4 < 15){
      distanceMeasurement();
      delay(10);
      counter++;
    }
    delay(300);
    stand();
    delay(100);
    leftRotation_90();
    delay(100);
    distanceMeasurement();
    forward();
    while(distance_sensor_4 < 15){
      distanceMeasurement();
      delay(10);
    }
    delay(300);
    stand();
    delay(100);
    leftRotation_90();
    delay(100);
    forward();
    delay(300);
    while(counter>=0){
      delay(10);
    }
    stand();
    delay(100);
    rightRotation_90();
    delay(100);
  }
  else if(richtung == "links"){
    leftRotation_90();
    delay(100);
    int counter = 0;
    distanceMeasurement();
    forward();
    while(distance_sensor_5 < 15){
      distanceMeasurement();
      delay(10);
      counter++;
    }
    delay(300);
    stand();
    delay(100);
    rightRotation_90();
    delay(100);
    distanceMeasurement();
    forward();
    while(distance_sensor_5 < 15){
      distanceMeasurement();
      delay(10);
    }
    delay(300);
    stand();
    delay(100);
    rightRotation_90();
    delay(100);
    forward();
    delay(300);
    while(counter>=0){
      delay(10);
    }
    stand();
    delay(100);
    leftRotation_90();
    delay(100);
  }
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
