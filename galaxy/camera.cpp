#include "camera.h"

#include <iostream>
//#include <__ostream/basic_ostream.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch),
      MovementSpeed(2.5f), MouseSensitivity(0.1f) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Forward, Up);
}

void Camera::ProcessKeyboard(char direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == 'W') Position += Forward * velocity;
    if (direction == 'S') Position -= Forward * velocity;
    if (direction == 'A') Position -= Right * velocity;
    if (direction == 'D') Position += Right * velocity;
    if (direction == 'U') Position += Up * velocity;
    if (direction == 'B') Position -= Up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 forward;
    forward.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    forward.y = sin(glm::radians(Pitch));
    forward.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Forward = glm::normalize(forward);
    Right = glm::normalize(glm::cross(Forward, WorldUp));
    Up = glm::normalize(glm::cross(Right, Forward));
}
