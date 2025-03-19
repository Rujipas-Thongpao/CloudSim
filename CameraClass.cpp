#include"CameraClass.h"


Camera::Camera(glm::vec3 _cameraPos, glm::vec3 _cameraFront, glm::vec3 _cameraUp)
{
    cameraPos = _cameraPos;
    cameraFront = _cameraFront;
    cameraUp = _cameraUp;
}

glm::mat4 Camera::GetView()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::GetPerspective()
{
    return glm::perspective(glm::radians(fov), 800.f / 800.f, .1f, 100.f);
}

glm::vec3 Camera::GetCameraPosition() {
    return cameraPos;
}


glm::vec3 Camera::GetLookPos() {
    return cameraFront;
}

void Camera::processInput(GLFWwindow* window, bool DEBUG)
{
    if (DEBUG) return;
    TimeUtils time;
    float cameraSpeed = .0005f * time.deltaTime();

    // move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
}

void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos, bool DEBUG)
{
    if (DEBUG) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // rotate
    glm::vec3 direction;

    yaw += xoffset;
    pitch += yoffset;

    pitch = std::max(std::min(89.0f, pitch), -89.0f);

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}



void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov = std::min(std::max(fov, 1.0f), 45.f);
}
