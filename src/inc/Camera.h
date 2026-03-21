/**
 * @file Camera.h
 * @brief 相机类定义 / Camera class definition
 * 
 * 基于 LearnOpenGL 的相机实现，支持 FPS 风格的相机控制。
 * Camera implementation based on LearnOpenGL, supporting FPS-style camera control.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief 相机移动方向枚举 / Camera movement direction enum
 * 
 * 定义相机可能的移动方向，用于抽象窗口系统特定的输入方法。
 * Defines possible camera movement directions, used to abstract window-system specific input methods.
 */
enum Camera_Movement {
    FORWARD,    ///< 前进 / Move forward
    BACKWARD,   ///< 后退 / Move backward
    LEFT,       ///< 左移 / Move left
    RIGHT       ///< 右移 / Move right
};

/**
 * @brief 默认相机参数 / Default camera values
 */
namespace CameraDefaults {
    constexpr float YAW         = -90.0f;   ///< 默认偏航角 / Default yaw
    constexpr float PITCH       =  0.0f;    ///< 默认俯仰角 / Default pitch
    constexpr float SPEED       =  2.5f;    ///< 默认移动速度 / Default movement speed
    constexpr float SENSITIVITY =  0.1f;    ///< 默认鼠标灵敏度 / Default mouse sensitivity
    constexpr float ZOOM        =  45.0f;   ///< 默认视野角度 / Default zoom (FOV)
    constexpr float NEAR_PLANE  =  0.1f;    ///< 默认近裁剪面 / Default near plane
    constexpr float FAR_PLANE   =  100.0f;  ///< 默认远裁剪面 / Default far plane
}

/**
 * @brief 相机类 / Camera class
 * 
 * 抽象相机类，处理输入并计算相应的欧拉角、向量和矩阵。
 * Abstract camera class that processes input and calculates corresponding 
 * Euler Angles, Vectors and Matrices for use in OpenGL.
 * 
 * 特性 / Features:
 * - FPS 风格的相机控制 / FPS-style camera control
 * - 键盘移动（WASD）/ Keyboard movement (WASD)
 * - 鼠标视角控制 / Mouse look
 * - 滚轮缩放 / Scroll zoom
 * 
 * 使用示例 / Usage example:
 * @code
 * Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
 * 
 * // 每帧更新 / Update every frame
 * camera.ProcessKeyboard(FORWARD, deltaTime);
 * camera.ProcessMouseMovement(xoffset, yoffset);
 * 
 * // 获取矩阵 / Get matrices
 * glm::mat4 view = camera.GetViewMatrix();
 * glm::mat4 projection = camera.GetProjectionMatrix(width, height);
 * @endcode
 */
class Camera
{
public:
    // ==================== 相机属性 / Camera Attributes ====================
    
    glm::vec3 Position;     ///< 相机位置 / Camera position
    glm::vec3 Front;        ///< 相机前方向 / Camera front direction
    glm::vec3 Up;           ///< 相机上方向 / Camera up direction
    glm::vec3 Right;        ///< 相机右方向 / Camera right direction
    glm::vec3 WorldUp;      ///< 世界上方向 / World up direction

    // ==================== 欧拉角 / Euler Angles ====================
    
    float Yaw;      ///< 偏航角 / Yaw angle
    float Pitch;    ///< 俯仰角 / Pitch angle

    // ==================== 相机选项 / Camera Options ====================
    
    float MovementSpeed;    ///< 移动速度 / Movement speed
    float MouseSensitivity; ///< 鼠标灵敏度 / Mouse sensitivity
    float Zoom;             ///< 视野角度（FOV）/ Field of view

    // ==================== 投影参数 / Projection Parameters ====================
    
    float NearPlane;    ///< 近裁剪面 / Near clipping plane
    float FarPlane;     ///< 远裁剪面 / Far clipping plane

    // ==================== 构造函数 / Constructors ====================

    /**
     * @brief 向量构造函数 / Vector constructor
     * @param position 相机位置 / Camera position
     * @param up 上方向向量 / Up direction vector
     * @param yaw 偏航角 / Yaw angle
     * @param pitch 俯仰角 / Pitch angle
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = CameraDefaults::YAW, 
           float pitch = CameraDefaults::PITCH);

    /**
     * @brief 标量构造函数 / Scalar constructor
     * @param posX 位置 X / Position X
     * @param posY 位置 Y / Position Y
     * @param posZ 位置 Z / Position Z
     * @param upX 上方向 X / Up direction X
     * @param upY 上方向 Y / Up direction Y
     * @param upZ 上方向 Z / Up direction Z
     * @param yaw 偏航角 / Yaw angle
     * @param pitch 俯仰角 / Pitch angle
     */
    Camera(float posX, float posY, float posZ, 
           float upX, float upY, float upZ, 
           float yaw, float pitch);

    // ==================== 矩阵获取 / Matrix Getters ====================

    /**
     * @brief 获取视图矩阵 / Get view matrix
     * 
     * 使用欧拉角和 LookAt 矩阵计算视图矩阵。
     * Calculates view matrix using Euler Angles and LookAt Matrix.
     * 
     * @return glm::mat4 视图矩阵 / View matrix
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * @brief 获取投影矩阵 / Get projection matrix
     * 
     * 使用当前 Zoom 值计算透视投影矩阵。
     * Calculates perspective projection matrix using current Zoom value.
     * 
     * @param width 视口宽度 / Viewport width
     * @param height 视口高度 / Viewport height
     * @return glm::mat4 投影矩阵 / Projection matrix
     */
    glm::mat4 GetProjectionMatrix(float width, float height) const;

    /**
     * @brief 获取投影矩阵（自定义近远裁剪面）/ Get projection matrix with custom near/far planes
     * @param width 视口宽度 / Viewport width
     * @param height 视口高度 / Viewport height
     * @param nearPlane 近裁剪面 / Near clipping plane
     * @param farPlane 远裁剪面 / Far clipping plane
     * @return glm::mat4 投影矩阵 / Projection matrix
     */
    glm::mat4 GetProjectionMatrix(float width, float height, float nearPlane, float farPlane) const;

    // ==================== 输入处理 / Input Processing ====================

    /**
     * @brief 处理键盘输入 / Process keyboard input
     * 
     * 处理来自任何键盘类输入系统的输入。
     * Processes input received from any keyboard-like input system.
     * 
     * @param direction 移动方向 / Movement direction
     * @param deltaTime 帧间隔时间 / Delta time between frames
     */
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    /**
     * @brief 处理鼠标移动 / Process mouse movement
     * 
     * 处理来自鼠标输入系统的输入。
     * Processes input received from a mouse input system.
     * 
     * @param xoffset X 轴偏移量 / X offset
     * @param yoffset Y 轴偏移量 / Y offset
     * @param constrainPitch 是否限制俯仰角 / Whether to constrain pitch
     */
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    /**
     * @brief 处理鼠标滚轮 / Process mouse scroll
     * 
     * 处理来自鼠标滚轮事件的输入。
     * Processes input received from a mouse scroll-wheel event.
     * 
     * @param yoffset 滚轮偏移量 / Scroll offset
     */
    void ProcessMouseScroll(float yoffset);

    // ==================== 设置器 / Setters ====================

    /**
     * @brief 设置相机位置 / Set camera position
     * @param position 新位置 / New position
     */
    void SetPosition(const glm::vec3& position);

    /**
     * @brief 设置相机朝向 / Set camera orientation
     * @param yaw 偏航角 / Yaw angle
     * @param pitch 俯仰角 / Pitch angle
     */
    void SetOrientation(float yaw, float pitch);

    /**
     * @brief 设置裁剪面 / Set clipping planes
     * @param nearPlane 近裁剪面 / Near clipping plane
     * @param farPlane 远裁剪面 / Far clipping plane
     */
    void SetClippingPlanes(float nearPlane, float farPlane);

    /**
     * @brief 设置移动速度 / Set movement speed
     * @param speed 移动速度 / Movement speed
     */
    void SetMovementSpeed(float speed);

    /**
     * @brief 设置鼠标灵敏度 / Set mouse sensitivity
     * @param sensitivity 鼠标灵敏度 / Mouse sensitivity
     */

    void SetMouseSensitivity(float sensitivity);

private:
    /**
     * @brief 更新相机向量 / Update camera vectors
     * 
     * 从相机的欧拉角计算前向量。
     * Calculates the front vector from the Camera's Euler Angles.
     */
    void updateCameraVectors();
};

#endif
