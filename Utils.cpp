#include "Utils.h"

// TODO : Create a utils class for this...
float fract(float f) {
    return f - floor(f);
}

float random(glm::vec3 st)
{
    // return 1.0;
    return fract(sin(glm::dot(st, glm::vec3(12.9898, 78.233, 32.43))) * 43758.5453123);
}

glm::vec3 mults(glm::vec3 v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}
