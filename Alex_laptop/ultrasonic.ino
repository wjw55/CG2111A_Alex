
#include "constants.h"
#include "packet.h"
#include <math.h>

void setup_ultrasonic(){
  //Let PC3 (pin 34) be TRIGGER pin, PC2 (pin 35) be ECHO pin
  DDRC |= (1 << 3);    
  DDRC &= ~(1 << 2);   
  PORTC &= ~(1 << 3);
}

uint32_t get_ultrasonic(){
  PORTC |= (1 << 3);    //set trigger pin high
  delayMicroseconds(10);
  PORTC &= ~(1 << 3);    //set trigger pin low
  unsigned long duration = pulseIn(echo_pin,HIGH);   //in microseconds   (can use timers)
  float dist = duration * 0.017;  //let Speed of sound be 340m/s
  return (uint32_t)round(dist);   //return distance in cm
}


void SendDist(uint32_t distance) {
  // put your main code here, to run repeatedly:
  TPacket Ultrasonic;
  Ultrasonic.packetType = PACKET_TYPE_RESPONSE;
  Ultrasonic.command = RESP_ULTRASONIC;
  Ultrasonic.params[0] = distance;
  sendResponse(&Ultrasonic);
}
