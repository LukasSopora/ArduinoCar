#include <NewPing.h>

//TODO: Adjust Pins
#define TRIG_PIN 1
#define ECHO_PIN_1 2
#define ECHO_PIN_2 3
#define ECHO_PIN_3 4
#define ECHO_PIN_4 5
#define ECHO_PIN_5 6
#define ECHO_PIN_6 7

//Ultrasonic Sensor max distance is 400cm
#define MAX_DISTANCE 400

unsigned int distance_sensor_1;
unsigned int distance_sensor_2;
unsigned int distance_sensor_3;
unsigned int distance_sensor_4;
unsigned int distance_sensor_5;
unsigned int distance_sensor_6;

//TODO: Check if you can define multiple sensors with same trigger pin
NewPing sensor_1(TRIG_PIN, ECHO_PIN_1, MAX_DISTANCE);
NewPing sensor_2(TRIG_PIN, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor_3(TRIG_PIN, ECHO_PIN_3, MAX_DISTANCE);
NewPing sensor_4(TRIG_PIN, ECHO_PIN_4, MAX_DISTANCE);
NewPing sensor_5(TRIG_PIN, ECHO_PIN_5, MAX_DISTANCE);
NewPing sensor_6(TRIG_PIN, ECHO_PIN_6, MAX_DISTANCE);

void setup() {
  Serial.begin(9600); 
}

void loop() {
  delay(50);

  distance_measurement();
}

//TODO: Check if delay between measurements is required
//TODO: Check sensor meas order
void distance_measurement() {
  distance_sensor_1 = sensor1.ping_cm();
  distance_sensor_2 = sensor2.ping_cm();
  distance_sensor_3 = sensor3.ping_cm();
  distance_sensor_4 = sensor4.ping_cm();
  distance_sensor_5 = sensor5.ping_cm();
  distance_sensor_6 = sensor6.ping_cm();
}
