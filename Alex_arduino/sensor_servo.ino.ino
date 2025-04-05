//servo
#define S0 53
#define S1 51
#define S2 47
#define S3 49
#define sensorOut A8
#define echo_pin 35



int readColor(int S2_state, int S3_state) {
    digitalWrite(S2, S2_state);
    digitalWrite(S3, S3_state);
    delay(200);  // Allow sensor to stabilize
    return pulseIn(sensorOut, HIGH);  // Measure frequency output
}
void setup_colorsensor(){
//  /setup_servo();
  Serial.begin(9600);
  /*void setupSerial();
  void startSerial();*/
  pinMode(S0,OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(41,HIGH);
}

void read_color() {
  // put your main code here, to run repeatedly:
  TPacket Color;
  Color.packetType = PACKET_TYPE_RESPONSE;
  Color.command = RESP_COLOR;
  /*float dist;
  dist = get_ultrasonic();
  /*char buffer[40];
  memcpy(buffer,&dist,40);
  writeSerial(buffer,40);*/

  int red = readColor(LOW, LOW);     // Read Red
  int green = readColor(HIGH, HIGH); // Read Green
  int blue = readColor(LOW, HIGH);   // Read Blue
//  Serial.print("R: "); Serial.print(red);
//  Serial.print(" G: "); Serial.print(green);
//  Serial.print(" B: "); Serial.print(blue);
//  Serial.print(".   Dist = "); Serial.println(dist);
  Color.params[0] = red;
  Color.params[1] = green;
  Color.params[2] = blue;
  sendResponse(&Color);
}
