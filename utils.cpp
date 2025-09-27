#include "config.h"
#include "math.h"

#ifndef PI
#define PI 3.14159265359
#endif

template<typename T>
T util_clamp(T value, T min, T max) {
  return (value < min) ? min : (value > max) ? max : value;
}

template<typename T>
T util_is_in_bound(T value, T min, T max) {
  if (value < min || value > max)
    return false;
  return true;
}

float util_map(float input, float min, float max, float newMin, float newMax) {
  return (input - min) * (newMax - newMin) / (max - min) + newMin;
}

float rad2deg(float rad) {
    return rad * 180 / PI;
}

int deg2microseconds(float deg) {
  int output = util_map(deg, 0, 180, servoLowMicroseconds, servoHighMicroseconds);
  return util_clamp(output, servoLowMicroseconds, servoHighMicroseconds);
}

bool xy2polar(float x, float y, float& r, float& theta)
{
  r = sqrt(x * x + y * y);

  if(r == 0) 
    return false;

  float c = util_clamp(x / r, -1.0f, 1.0f);
  float s = util_clamp(y / r, -1.0f, 1.0f);

  theta = acos(c);
  if(s < 0) 
    theta *= -1;

  return true;
}

// find angle using cosine rule
bool find_angle(float opp_edge, float adj_edge1, float adj_edge2, float& theta)
{
    float den = 2 * adj_edge1 * adj_edge2;
    if(den==0) 
      return false;

    float c = (adj_edge1 * adj_edge1 + adj_edge2 * adj_edge2 - opp_edge * opp_edge) / den;

    if(!util_is_in_bound(c, -1.0f, 1.0f))
      return false;

    theta = acos(c);
    return true;
}


bool r_z2shoulder_elbow(float r, float z, float& shoulderAngle, float& elbowAngle) {

  r -= L3;

  float theta, R;
  if(!xy2polar(r, z, R, theta))
    return false;

  float A, B;
  if(!find_angle(L2, L1, R, A)) 
    return false;
  if(!find_angle(R, L1, L2, B)) 
    return false;

  float shoulderAngleFromHorizAxis = theta + A;
  float ElbowAngleFromHorizAxis = B + shoulderAngleFromHorizAxis;

  shoulderAngle = shoulderMinPos + rad2deg(shoulderAngleFromHorizAxis);
  elbowAngle = 180 - rad2deg(ElbowAngleFromHorizAxis) + 2 * elbowMinPos;

  return true;
}