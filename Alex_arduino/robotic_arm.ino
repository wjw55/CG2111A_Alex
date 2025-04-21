#include <Servo.h>
Servo servoLeft;
Servo servoRight;
Servo disp;

void setup_servo(){
  servoLeft.attach(9);
  servoRight.attach(10);
  disp.attach(43);
  disp.write(130);  
}

void close_claw(){
  servoLeft.write(160);
  servoRight.write(20);
}

void open_claw(){
  servoLeft.write(90);
  servoRight.write(90);
}

void dispense(){
  disp.write(60);
}
