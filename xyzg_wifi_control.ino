#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "config.h"
#include "utils.h"
#include "math.h"

int gripperPin = 33;      // Claw
int elbowPin = 25;        // Right
int shoulderPin = 26;    // Left
int basePin = 27;        // Center

Servo gripperServo;
Servo elbowServo;
Servo shoulderServo;
Servo baseServo;

float gripperPos=gripperInitPos;
float basePos=baseInitPos;
float rPos=rInitPos;
float zPos=zInitPos;

const char* ssid     = "your-WiFi-name";
const char* password = "your-WiFi-password";

WiFiUDP udp;
unsigned int localUdpPort = 8888;
char incomingPacket[256];

void write_servo(float input, Servo &servo, float &pos, float min, float max, float velocity, float ignore_ratio);

void elbow_shoulder_control(float r_input, float z_input, float ignore_ratio);

void parse_float(String requestLine, float& baseInput, float& rInput, float& zInput, float& gripperInput);

void control(float baseInput, float rInput, float zInput, float gripperInput);

void setup(){
  Serial.begin(9600);
  
  // WiFi setup
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  udp.begin(localUdpPort);
  
  // servo initialization
  gripperServo.attach(gripperPin);
  elbowServo.attach(elbowPin);
  shoulderServo.attach(shoulderPin);
  baseServo.attach(basePin);

  gripperServo.writeMicroseconds(deg2microseconds(gripperPos));
  delay(200);

  baseServo.writeMicroseconds(deg2microseconds(basePos));
  delay(200);

  float shoulderPos, elbowPos;
  if(!r_z2shoulder_elbow(rInitPos, zInitPos, shoulderPos, elbowPos))
    return;

  shoulderServo.writeMicroseconds(deg2microseconds(shoulderPos));
  delay(200);
  elbowServo.writeMicroseconds(deg2microseconds(elbowPos));
  delay(200);
}

void loop() {
  float baseInput=0, rInput=0, zInput=0, gripperInput=0;

  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if(DEBUG) {
      Serial.println(incomingPacket);
    }

    if (len > 0) {
      incomingPacket[len] = 0;
      parse_float(incomingPacket, baseInput, rInput, zInput, gripperInput);
      control(baseInput, rInput, zInput, gripperInput);
    }
  }
  delay(20);
}

void control(float baseInput, float rInput, float zInput, float gripperInput) {
  write_servo(gripperInput, gripperServo, gripperPos, gripperMinPos, gripperMaxPos, gripperVelocity, 4.5);
  write_servo(baseInput, baseServo, basePos, baseMinPos, baseMaxPos, baseVelocity, 2.5);

  elbow_shoulder_control(rInput, zInput, 2.5);
}

void write_servo(float input, Servo &servo, float &pos, float min, float max, float velocity, float ignore_ratio) {
  if(abs(input) < ignore_ratio)
    return;

  pos += input * velocity;
  pos = util_clamp(pos, min, max);
  
  servo.writeMicroseconds(deg2microseconds(pos));
}

void elbow_shoulder_control(float r_input, float z_input, float ignore_ratio) {
  float prev_rPos = rPos;
  float prev_zPos = zPos;

  if(abs(r_input) >= ignore_ratio)
    rPos += r_input * rVelocity;
  else if(abs(z_input) >= ignore_ratio)
    zPos += z_input * zVelocity;
  else
    return;

  rPos = util_clamp(rPos, rMinPos, rMaxPos);
  zPos = util_clamp(zPos, zMinPos, zMaxPos);

  float shoulderPos, elbowPos;
  bool is_reacheable = r_z2shoulder_elbow(rPos, zPos, shoulderPos, elbowPos);
  if(!is_reacheable || 
  !util_is_in_bound(shoulderPos, shoulderMinPos, shoulderMaxPos) ||
  !util_is_in_bound(elbowPos, elbowMinPos, elbowMaxPos)
  ){
    rPos = prev_rPos;
    zPos = prev_zPos;
    return;
  }
  
  shoulderServo.writeMicroseconds(deg2microseconds(shoulderPos));
  elbowServo.writeMicroseconds(deg2microseconds(elbowPos));

  if(DEBUG) {
    Serial.print(shoulderPos);
    Serial.print("\t\t");
    Serial.print(elbowPos);
    Serial.print("\t\t");
    Serial.print(rPos);
    Serial.print("\t\t");
    Serial.print(zPos);
    Serial.print("\t\t");
    Serial.print("\n");
  }
}

void parse_float(String data, float& baseInput, float& rInput, float& zInput, float& gripperInput) {
  baseInput = rInput = zInput = gripperInput = 0;
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  int thirdComma = data.indexOf(',', secondComma + 1);
  
  if (firstComma != -1 && secondComma != -1 && thirdComma != -1) {
      baseInput = data.substring(0, firstComma).toFloat();
      rInput = data.substring(firstComma + 1, secondComma).toFloat();
      zInput = data.substring(secondComma + 1, thirdComma).toFloat();
      gripperInput = data.substring(thirdComma + 1).toFloat();
  }
}