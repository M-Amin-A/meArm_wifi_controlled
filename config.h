#ifndef CONFIG_H
#define CONFIG_H

const bool DEBUG=false;

const float L0=50;   // base height mm
const float L1=80;   // arm1 length mm
const float L2=80;   // arm2 length mm
const float L3=70;   // gripper length mm

const int servoLowMicroseconds=700;
const int servoHighMicroseconds=2300;

const float gripperVelocity = 2.0;
const float gripperMinPos=30;
const float gripperMaxPos=180;
const float gripperInitPos=150;

const float baseVelocity = 0.5;
const float baseMinPos=0;
const float baseMaxPos=180;
const float baseInitPos=90;

const float elbowVelocity = 0.5;
const float elbowMinPos=30;
const float elbowMaxPos=150;
const float elbowInitPos=150;

const float shoulderVelocity = 0.5;
const float shoulderMinPos=90;
const float shoulderMaxPos=180;
const float shoulderInitPos=120;

const float rVelocity = 1;
const float rMinPos = L3;
const float rMaxPos = L1 + L2 + L3;
const float rInitPos = 2 * L3;

const float zVelocity = 1;
const float zMinPos = -L0;
const float zMaxPos = L1;
const float zInitPos = 0;

#endif // CONFIG_H
