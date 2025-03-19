#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <iostream>

#include "TimeClass.h"

class Camera
{
public:
    glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
    float yaw = 0.f;
    float pitch = 0.f;
    float fov = 45.f;

    Camera(glm::vec3 _cameraPos, glm::vec3 _cameraFront, glm::vec3 _cameraUp);

    glm::mat4 GetView();

    glm::mat4 GetPerspective();

    glm::vec3 GetCameraPosition();

    glm::vec3 GetLookPos();

    void processInput(GLFWwindow* window, bool DEBUG);

    void mouse_callback(GLFWwindow* window, double xpos, double ypos, bool DEBUG);

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


private:
    bool firstMouse = true;
    double lastX = 400.f, lastY = 300.f;

};