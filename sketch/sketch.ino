#include <NewPing.h>
#include "stats.h"

//Ultrasonic Trigger Pins
#define TRIG_PIN_1 53
#define TRIG_PIN_2 47
#define TRIG_PIN_3 45
#define TRIG_PIN_4 4
#define TRIG_PIN_5 2

//Ultrasonic Echo Pins
#define ECHO_PIN_1 51
#define ECHO_PIN_2 49
#define ECHO_PIN_3 43
#define ECHO_PIN_4 5
#define ECHO_PIN_5 3 

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
#define DEFAULT_DISTANCE 401
#define CRITICAL_DISTANCE_FRONT 22
#define CRITICAL_DISTANCE_FRONT_LEFT 35
#define CRITICAL_DISTANCE_FRONT_RIGHT 35

//Color Sensor TCS32000 Configuration
#define TCS_S0 13
#define TCS_S1 12
#define TCS_S2 11
#define TCS_S3 10
#define TCS_sensor_out 9

//Light Sensor Configuration
#define LED_PIN 33
#define LIGHTSENSOR_PIN A2
#define LIGHT_THRESHOLD 100

//RGB ratio for color recognizion
#define COLOR_RATIO 0.7

//Acceleration Configuration
#define ACCELERATION_MAX_ITER 20
#define ACCELERATION_PAUSE 200
#define LEFT_ROTATION_90_DELAY 650
#define RIGHT_ROTATION_90_DELAY 630
#define MOTOR_DELAY 100
#define BACKWARD_DELAY 500

//Dodge Distance Constants
#define DODGE_DISTANCE_MAX_ITER 15
#define DODGE_DISTANCE_MIN_ITER 17

//Distances
int distance_sensor_1;
int distance_sensor_2;
int distance_sensor_3;
int distance_sensor_4;
int distance_sensor_5;

//Stats
Driving_State driving_state;
Object_State object_State = nothing;
Colors tcs_color = color_NONE;
Dodge_State dodge_state = dodge_NONE;

//Ultrasonic Sensors
NewPing sensor_1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor_2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor_3(TRIG_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sensor_4(TRIG_PIN_4, ECHO_PIN_4, MAX_DISTANCE);
NewPing sensor_5(TRIG_PIN_5, ECHO_PIN_5, MAX_DISTANCE);

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

//Light Sensor Methods
void initLightSensor();

//Color Sensor Variables
int freqRed = 0;
int freqGreen = 0;
int freqBlue = 0;

double ratioRed = 0;
double ratioBlue = 0;

//Speed Variables
int accelerating_speed = 128;
int accelerate_counter = 0;
int driving_speed = 128;

//Dodhe Variables
int dodgeFreeCounter = 0;
int dodgeDistanceCounter = 0;
int dodgeMinCounter = 0;

//Light Sensor Variables
int lightVal = 0;

void setup() {
  setMotorPinModes();

  driving_state = standing;
  object_State = nothing;
  dodge_state = dodge_NONE;

  initColorSensor();

  initLightSensor();

  Serial.begin(9600);
}

void loop() {

  distanceMeasurement();
  readColor();

  //Automatic lights
  lightVal = analogRead(LIGHTSENSOR_PIN);
  if(lightVal > LIGHT_THRESHOLD) {
    digitalWrite(LED_PIN, LOW);
  }
  else {
    digitalWrite(LED_PIN, HIGH);
  }

  Serial.println(distance_sensor_2);
  
  switch (dodge_state)
  {
    case dodge_left_step_1:
      Serial.println("DL 1");
      dodgeLeft1();
      break;
    case dodge_left_step_2:
      Serial.println("DL 2");  
      dodgeLeft2();
      break;
    case dodge_left_step_3:
      Serial.println("DL 3");
      dodgeLeft3(); 
      break;
    case dodge_right_step_1: 
      Serial.println("DR 1");
      dodgeRight1(); 
      break;
    case dodge_right_step_2: 
      Serial.println("DR 2");
      dodgeRight2(); 
      break;
    case dodge_right_step_3:
      Serial.println("DR 3");
      dodgeRight3(); 
      break;
  default:
    break;
  }

  if(dodge_state == dodge_NONE) {
    if(distance_sensor_2 < CRITICAL_DISTANCE_FRONT && driving_state != standing) {
      stand();
      if(distance_sensor_1 < CRITICAL_DISTANCE_FRONT_LEFT && distance_sensor_3 < CRITICAL_DISTANCE_FRONT_RIGHT){
        //Road blocked
        Serial.println("Stop");
      }
      else if(distance_sensor_1 > CRITICAL_DISTANCE_FRONT_LEFT){
        //Left part free
        Serial.println("Links vorbeifahren");
        dodgeLeft1();
        //vorbeifahren("rechts");
      }
      else if(distance_sensor_3 > CRITICAL_DISTANCE_FRONT_RIGHT){
        //Right part free
        Serial.println("Rechts vorbeifahren");
        dodgeRight1();
        //vorbeifahren("links");
      }
  }
  else if(distance_sensor_2 > CRITICAL_DISTANCE_FRONT && driving_state != straight) {
    Serial.println("Start");
    forward();
    driving_state = straight;
  }

  switch (tcs_color)
  {
    case color_red: 
      Serial.println("Red");
      leftCorrection();
      break;
    case color_blue:
      Serial.println("Blue");
      rightCorrection();
      break;
    case color_NONE:
      if(driving_state != straight) {
        forward();
      }
      //Serial.println("None");
    default: Serial.println("Fail"); break;
  }
  }
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

void distanceMeasurement() {
  distance_sensor_1 = sensor_1.ping_cm();
  distance_sensor_2 = sensor_2.ping_cm();
  distance_sensor_3 = sensor_3.ping_cm();
  distance_sensor_4 = sensor_4.ping_cm();
  distance_sensor_5 = sensor_5.ping_cm();

  if(distance_sensor_1 == 0) {
    distance_sensor_1 = DEFAULT_DISTANCE;
  }
  if(distance_sensor_2 == 0) {
    distance_sensor_2 = DEFAULT_DISTANCE;
  }
  if(distance_sensor_3 == 0) {
    distance_sensor_3 = DEFAULT_DISTANCE;
  }
  if(distance_sensor_4 == 0) {
    distance_sensor_4 = DEFAULT_DISTANCE;
  }
  if(distance_sensor_5 == 0) {
    distance_sensor_5 = DEFAULT_DISTANCE;
  }
}

#pragma region Color Sensor Methods
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

void initLightSensor() {
  pinMode(LIGHTSENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

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
    dodgeMinCounter = 0;
    forward();
  }
  else {
    //Working case

    dodgeMinCounter++;

    if(dodgeMinCounter >= DODGE_DISTANCE_MIN_ITER && distance_sensor_5 > 35) {
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

void dodgeRight1() {
  if(dodge_state == dodge_NONE && driving_state == standing) {
    Serial.println("Init DR 1");
    //Initial Case
    rightRotation_90();
    forward();
    dodgeDistanceCounter = 0;
    dodgeFreeCounter = 0;
    dodge_state = dodge_right_step_1;
  }
  else if(dodge_state == dodge_right_step_1) {
    Serial.println("Working DR 1");
    
    //Working Case
    dodgeDistanceCounter++;

    if(distance_sensor_4 > 15) {
      dodgeFreeCounter++;
    }

    if(dodgeFreeCounter >= DODGE_DISTANCE_MAX_ITER) {
      Serial.println("END DR 1");
      stand();
      delay(MOTOR_DELAY);
      leftRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_right_step_2;
    }
  }
}
void dodgeRight2() {
  if(driving_state == standing) {
    Serial.println("Init DR 2");
    //Initial Case
    dodgeFreeCounter = 0;
    dodgeMinCounter = 0;
    forward();
  }
  else {
    //Working case

    dodgeMinCounter ++;

    Serial.println("Working DR 2");

    if(dodgeMinCounter >= DODGE_DISTANCE_MIN_ITER && distance_sensor_4 > 35) {
      dodgeFreeCounter++;
    }

    if(dodgeFreeCounter >= DODGE_DISTANCE_MAX_ITER) {
      Serial.println("End DR 2");
      stand();
      delay(MOTOR_DELAY);
      leftRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_right_step_3;
    }
  }
}
void dodgeRight3() {
  if(driving_state == standing) {
    Serial.println("Init DR 3");
    //Initial case
    forward();
  }
  else {
    //Working case
    Serial.println("Working DR 3");
    dodgeDistanceCounter--;

    if(dodgeDistanceCounter <= 0) {
      Serial.println("End DR 3");
      stand();
      delay(MOTOR_DELAY);
      rightRotation_90();
      delay(MOTOR_DELAY);
      dodge_state = dodge_NONE;
    }
  }
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

  delay(LEFT_ROTATION_90_DELAY);

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

  delay(RIGHT_ROTATION_90_DELAY);

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
