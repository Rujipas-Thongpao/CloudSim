#pragma once
#ifndef INCLUDE_TIME_UTILS
#define INCLUDE_TIME_UTILS

#include <GLFW/glfw3.h>

class TimeUtils
{
public:
    double GetTime = glfwGetTime();
    double lastFrame = 0.0f;

    float deltaTime();
};

#endif