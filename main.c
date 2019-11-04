//Motor 1
const int motor1_forw = 10;
const int motor1_backw = 11;
//Motor 2
const int motor2_forw = 12;
const int motor2_backw = 13;
//Motor 3
const int motor3_forw = 3;
const int motor3_backw = 2;
//Motor 4
const int motor4_forw = 1;
const int motor4_backw = 0;

int speed = 255;

void setup(){
    //Set pins as outputs
    pinMode(motor1_forw, OUTPUT);
    pinMode(motor1_backw, OUTPUT);
    pinMode(motor2_forw, OUTPUT);
    pinMode(motor2_backw, OUTPUT);
    pinMode(motor3_forw, OUTPUT);
    pinMode(motor3_backw, OUTPUT);
    pinMode(motor4_forw, OUTPUT);
    pinMode(motor4_backw, OUTPUT);


    //delay(2000);
    analogWrite(motor1_forw, speed);
    analogWrite(motor2_forw, speed);
    analogWrite(motor3_forw, speed);
    analogWrite(motor4_forw, speed);
}

void loop(){
}