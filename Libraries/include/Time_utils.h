#ifndef INCLUDE_TIME_UTILS
#define INCLUDE_TIME_UTILS

#include <GLFW/glfw3.h>

class TimeUtils
{
public:
    double GetTime = glfwGetTime();
    double lastFrame = 0.0f;

    float deltaTime()
    {
        float deltaTime = GetTime - lastFrame;
        lastFrame = GetTime;
        return deltaTime;
    }
};

#endif