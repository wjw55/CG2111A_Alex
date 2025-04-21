//servo
#define S0 53
#define S1 51
#define S2 47
#define S3 49
#define sensorOut 29
#define LEDpin 39


int readColor(int S2_state, int S3_state) {
    digitalWrite(LEDpin,HIGH);
    digitalWrite(S2, S2_state);
    digitalWrite(S3, S3_state);
    delay(200);  // Allow sensor to stabilize
    while (sensorOut == LOW);
    int duration = pulseIn(sensorOut, LOW);
    digitalWrite(LEDpin,LOW);
    delay(100);
    return duration;  // Measure frequency output   (Lower duration <=> Higher frequency <=> Higher colour intensity)
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
  pinMode(39,OUTPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(41,HIGH);
}

float colourDistance(int r1, int g1, int b1, int r2, int g2, int b2) {
  return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

void detect_color(int r, int g, int b, TPacket *Color) {
  uint32_t d_red = (uint32_t)colourDistance(r, g, b, 691, 860, 680);
  uint32_t d_green = (uint32_t)colourDistance(r, g, b, 1173, 400, 1170);

  if (d_red < d_green) {
    Color->params[3] = 0; //red astronaut detected
  } else {
    Color->params[3] = 1; //green astronaut detected
  }
  Color->params[4] = d_red; // distance from red
  Color->params[5] = d_green; //distance from green
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
  detect_color(red, green, blue, &Color);
  sendResponse(&Color);
}
