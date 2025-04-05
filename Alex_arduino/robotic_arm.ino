#include <Servo.h>
Servo servoLeft;
Servo servoRight;

void setup_servo(){
  servoLeft.attach(9);
  servoRight.attach(10);
  open_claw();
}

void close_claw(){
  servoLeft.write(180);
  servoRight.write(10);
}

void open_claw(){
  servoLeft.write(110);
  servoRight.write(70);
}
