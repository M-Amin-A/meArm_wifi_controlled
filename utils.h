#ifndef UTILS_H
#define UTILS_H

#include "config.h"
#include "math.h"

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

float util_map(float input, float min, float max, float newMin, float newMax);

bool r_z2shoulder_elbow(float r, float z, float& shoulderAngle, float& elbowAngle);

int deg2microseconds(float deg);

#endif // UTILS_H
