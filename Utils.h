#pragma once
#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


float fract(float f);

float random(glm::vec3 st);

glm::vec3 mults(glm::vec3 v, float s);

#endif

