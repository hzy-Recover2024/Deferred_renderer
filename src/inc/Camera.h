/**
 * @file Camera.h
 * @brief FPS 风格相机类
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

namespace CameraDefaults {
    constexpr float YAW         = -90.0f;
    constexpr float PITCH       =  0.0f;
    constexpr float SPEED       =  2.5f;
    constexpr float SENSITIVITY =  0.1f;
    constexpr float ZOOM        =  45.0f;
    constexpr float NEAR_PLANE  =  0.1f;
    constexpr float FAR_PLANE   =  100.0f;
}

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float NearPlane;
    float FarPlane;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = CameraDefaults::YAW,
           float pitch = CameraDefaults::PITCH);

    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float width, float height) const;
    glm::mat4 GetProjectionMatrix(float width, float height, float nearPlane, float farPlane) const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

    void SetPosition(const glm::vec3& position);
    void SetOrientation(float yaw, float pitch);
    void SetClippingPlanes(float nearPlane, float farPlane);
    void SetMovementSpeed(float speed);
    void SetMouseSensitivity(float sensitivity);

private:
    void updateCameraVectors();
};

#endif
