/**
 * @file Camera.cpp
 * @brief 相机类实现
 */

#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(CameraDefaults::SPEED),
      MouseSensitivity(CameraDefaults::SENSITIVITY),
      Zoom(CameraDefaults::ZOOM),
      NearPlane(CameraDefaults::NEAR_PLANE),
      FarPlane(CameraDefaults::FAR_PLANE)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(CameraDefaults::SPEED),
      MouseSensitivity(CameraDefaults::SENSITIVITY),
      Zoom(CameraDefaults::ZOOM),
      NearPlane(CameraDefaults::NEAR_PLANE),
      FarPlane(CameraDefaults::FAR_PLANE)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
{
    return glm::perspective(glm::radians(Zoom), width / height, NearPlane, FarPlane);
}

glm::mat4 Camera::GetProjectionMatrix(float width, float height, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(Zoom), width / height, nearPlane, farPlane);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::SetPosition(const glm::vec3& position)
{
    Position = position;
}

void Camera::SetOrientation(float yaw, float pitch)
{
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::SetClippingPlanes(float nearPlane, float farPlane)
{
    NearPlane = nearPlane;
    FarPlane = farPlane;
}

void Camera::SetMovementSpeed(float speed)
{
    MovementSpeed = speed;
}

void Camera::SetMouseSensitivity(float sensitivity)
{
    MouseSensitivity = sensitivity;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
