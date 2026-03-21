/**
 * @file Camera.cpp
 * @brief 相机类实现 / Camera class implementation
 */

#include "Camera.h"

/**
 * @brief 向量构造函数实现 / Vector constructor implementation
 */
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

/**
 * @brief 标量构造函数实现 / Scalar constructor implementation
 */
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

/**
 * @brief 获取视图矩阵 / Get view matrix
 */
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

/**
 * @brief 获取投影矩阵 / Get projection matrix
 */
glm::mat4 Camera::GetProjectionMatrix(float width, float height) const
{
    return glm::perspective(glm::radians(Zoom), width / height, NearPlane, FarPlane);
}

/**
 * @brief 获取投影矩阵（自定义近远裁剪面）/ Get projection matrix with custom near/far planes
 */
glm::mat4 Camera::GetProjectionMatrix(float width, float height, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(Zoom), width / height, nearPlane, farPlane);
}

/**
 * @brief 处理键盘输入 / Process keyboard input
 */
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

/**
 * @brief 处理鼠标移动 / Process mouse movement
 */
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

/**
 * @brief 处理鼠标滚轮 / Process mouse scroll
 */
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

/**
 * @brief 设置相机位置 / Set camera position
 */
void Camera::SetPosition(const glm::vec3& position)
{
    Position = position;
}

/**
 * @brief 设置相机朝向 / Set camera orientation
 */
void Camera::SetOrientation(float yaw, float pitch)
{
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

/**
 * @brief 设置裁剪面 / Set clipping planes
 */
void Camera::SetClippingPlanes(float nearPlane, float farPlane)
{
    NearPlane = nearPlane;
    FarPlane = farPlane;
}

/**
 * @brief 设置移动速度 / Set movement speed
 */
void Camera::SetMovementSpeed(float speed)
{
    MovementSpeed = speed;
}

/**
 * @brief 设置鼠标灵敏度 / Set mouse sensitivity
 */
void Camera::SetMouseSensitivity(float sensitivity)
{
    MouseSensitivity = sensitivity;
}

/**
 * @brief 更新相机向量 / Update camera vectors
 * 
 * 从相机的欧拉角计算前向量。
 * Calculates the front vector from the Camera's Euler Angles.
 */
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
