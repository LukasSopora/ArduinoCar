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
#define ROTATION_90_DELAY 1900
#define MOTOR_DELAY 100
#define BACKWARD_DELAY 500

//Dodge Distance Constants
#define DODGE_DISTANCE_MAX_ITER 300

//Distances
int distance_sensor_1;
int distance_sensor_2;
int distance_sensor_3;
int distance_sensor_4;
int distance_sensor_5;
int distance_sensor_6;

//Stats
Driving_State driving_state;
Object_State object_State = nothing;
Colors tcs_color = color_NONE;
Dodge_State dodge_state = dodge_NONE;

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
void leftCorrection();
void rightCorrection();

//Dodge Methods
void dodgeRight1();
void dodgeRight2();
void dodgeRight3();

void dodgeLeft1();
void dodgeLeft2();
void dodgeLeft3();

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

//Dodhe Variables
int dodgeFreeCounter = 0;
int dodgeDistanceCounter = 0;

void setup() {
  setMotorPinModes();

  driving_state = standing;
  object_State = nothing;

  initColorSensor();

  Serial.begin(9600);
}

void loop() {
  distanceMeasurement();

  readColor();

  switch (tcs_color)
  {
  case color_red: Serial.println("Red"); break;
  case color_blue: Serial.println("Blue"); break;
  case color_NONE: Serial.println("None"); break;
  
  default: Serial.println("Fail"); break;
  }
  
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
}

void print_states() {
  Serial.print("Driving State: ");
  Serial.println(driving_state);

  Serial.print("Object State: ");
  Serial.println(object_State);

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

void leftCorrection() {
  stand();
  delay(MOTOR_DELAY);
  backward();
  delay(BACKWARD_DELAY);
  stand();
  delay(MOTOR_DELAY);
  leftRotation_90();
  delay(MOTOR_DELAY);
}

void rightCorrection() {
  stand();
  delay(MOTOR_DELAY);
  backward();
  delay(BACKWARD_DELAY);
  stand();
  delay(MOTOR_DELAY);
  rightRotation_90();
  delay(MOTOR_DELAY);
}

void dodgeLeft1() {
  if(dodge_state == dodge_NONE && driving_state == standing) {
    //Initial Case
    leftRotation_90();
    forward();
    dodgeDistanceCounter = 0;
    dodgeFreeCounter = 0;
    dodge_state = dodge_left_step_1;
  }
  else if(dodge_state == dodge_left_step_1) {
    
    //Working Case
    dodgeDistanceCounter++;

    if(distance_sensor_5 > 15) {
      dodgeFreeCounter++;
    }

    if(dodgeFreeCounter >= DODGE_DISTANCE_MAX_ITER) {
      stand();
      delay(MOTOR_DELAY);
      rightRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_left_step_2;
    }
  }
}
void dodgeLeft2() {
  if(driving_state == standing) {
    //Initial Case
    dodgeFreeCounter = 0;
    forward();
  }
  else {
    //Working case
    if(distance_sensor_5 > 15) {
      dodgeFreeCounter++;
    }

    if(dodgeFreeCounter >= DODGE_DISTANCE_MAX_ITER) {
      stand();
      delay(MOTOR_DELAY);
      rightRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_left_step_3;
    }
  }
}

void dodgeLeft3() {
  if(driving_state == standing) {
    //Initial case
    forward();
  }
  else {
    //Working case
    dodgeDistanceCounter--;

    if(dodgeDistanceCounter <= 0) {
      stand();
      delay(MOTOR_DELAY);
      leftRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_NONE;
    }
  }
}

void dodgeRight1() {}
void dodgeRight2() {}
void dodgeRight3() {}

void leftRotation_90() {
  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR_FL_BACKW, driving_speed);

  analogWrite(MOTOR_FR_FORW, driving_speed);
  analogWrite(MOTOR_FR_BACKW, 0);

  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, driving_speed);

  analogWrite(MOTOR_BR_FORW, driving_speed);
  analogWrite(MOTOR_BR_BACKW, 0);

  delay(ROTATION_90_DELAY);

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

  delay(ROTATION_90_DELAY);

  analogWrite(MOTOR_FL_FORW, 0);
  analogWrite(MOTOR_FL_BACKW, 0);

  analogWrite(MOTOR_FR_FORW, 0);
  analogWrite(MOTOR_FR_BACKW, 0);

  analogWrite(MOTOR_BL_FORW, 0);
  analogWrite(MOTOR_BL_BACKW, 0);

  analogWrite(MOTOR_BR_FORW, 0);
  analogWrite(MOTOR_BR_BACKW, 0);
}

void forward() {
  motor_1_forw(driving_speed);
  motor_2_forw(driving_speed);
  motor_3_forw(driving_speed);
  motor_4_forw(driving_speed);

  driving_state = straight;
}

void backward() {
  motor_1_backw(accelerating_speed);
  motor_2_backw(accelerating_speed);
  motor_3_backw(accelerating_speed);
  motor_4_backw(accelerating_speed);
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
