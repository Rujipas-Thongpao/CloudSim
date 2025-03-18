#include "TimeClass.h"

float TimeUtils::deltaTime()
{
    float deltaTime = GetTime - lastFrame;
    lastFrame = GetTime;
    return deltaTime;
}
