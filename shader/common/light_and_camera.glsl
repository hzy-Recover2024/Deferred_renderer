/**
 * @file light_and_camera.glsl
 * @brief 光照和相机数据结构 / Light and camera data structures
 * 
 * 定义光照和相机的 Uniform 结构体，针对 std140 布局优化内存。
 * Defines uniform structures for lighting and camera, optimized for std140 layout.
 */

#ifndef LIGHT_AND_CAMERA_GLSL
#define LIGHT_AND_CAMERA_GLSL

#include "uniform_layout.h"



// ==================== 光源类型常量 / Light Type Constants ====================

const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;
const int LIGHT_TYPE_SPOT        = 2;

// ==================== 光源结构体（std140 优化）/ Light Structure (std140 optimized) ====================

/**
 * @brief 通用光源结构体 / Generic light structure
 * 
 * 使用 vec4 打包数据，优化 std140 内存布局（80 字节）。
 * Uses vec4 packing for optimized std140 memory layout (80 bytes).
 * 
 * 内存布局 / Memory layout:
 * - vec4 colorIntensity:     xyz = color, w = intensity
 * - vec4 positionInvRadius:  xyz = position, w = invRadius
 * - vec4 directionType:      xyz = direction, w = type (as float)
 * - vec4 params:             x = innerAngle, y = outerAngle, z = colorTemperature, w = sourceRadius
 * - int castShadows:         是否投射阴影 / Cast shadows flag
 * - int padding[3]:          内存对齐填充 / Memory alignment padding
 */
struct Light
{
    vec4 colorIntensity;        ///< xyz = 颜色, w = 强度 / xyz = color, w = intensity
    vec4 positionInvRadius;     ///< xyz = 位置, w = 半径倒数 / xyz = position, w = invRadius
    vec4 directionType;         ///< xyz = 方向, w = 类型 / xyz = direction, w = type
    vec4 params;                ///< x = 内锥角, y = 外锥角, z = 色温, w = 光源半径
                                ///< x = innerAngle, y = outerAngle, z = colorTemp, w = sourceRadius
    int castShadows;            ///< 是否投射阴影 / Cast shadows flag
    int padding1;               ///< 内存对齐 / Memory alignment
    int padding2;
    int padding3;
};

// ==================== 相机结构体 / Camera Structure ====================

/**
 * @brief 相机结构体 / Camera structure
 * 
 * 内存布局（400 字节）/ Memory layout (400 bytes):
 * - vec4 positionNear:       xyz = 位置, w = 近裁剪面
 * - vec4 forwardFar:         xyz = 前向, w = 远裁剪面
 * - mat4 viewMatrix:         视图矩阵
 * - mat4 projectionMatrix:   投影矩阵
 * - mat4 viewProjectionMatrix: 视图投影矩阵
 * - mat4 invViewMatrix:      视图矩阵逆矩阵
 * - mat4 invProjectionMatrix: 投影矩阵逆矩阵
 * - mat4 invViewProjectionMatrix: 视图投影矩阵逆矩阵
 */
struct Camera
{
    vec4 positionNear;              ///< xyz = 位置, w = 近裁剪面 / xyz = position, w = nearPlane
    vec4 forwardFar;                ///< xyz = 前向, w = 远裁剪面 / xyz = forward, w = farPlane
    mat4 viewMatrix;                ///< 视图矩阵 / View matrix
    mat4 projectionMatrix;          ///< 投影矩阵 / Projection matrix
    mat4 viewProjectionMatrix;      ///< 视图投影矩阵 / View-projection matrix
    mat4 invViewMatrix;             ///< 视图矩阵逆矩阵 / Inverse view matrix
    mat4 invProjectionMatrix;       ///< 投影矩阵逆矩阵 / Inverse projection matrix
    mat4 invViewProjectionMatrix;   ///< 视图投影矩阵逆矩阵 / Inverse view-projection matrix
};

// ==================== Light 类型判断函数 / Light Type Check Functions ====================

bool isDirectionalLight(Light light) { return getLightType(light) == LIGHT_TYPE_DIRECTIONAL; }
bool isPointLight(Light light) { return getLightType(light) == LIGHT_TYPE_POINT; }
bool isSpotLight(Light light) { return getLightType(light) == LIGHT_TYPE_SPOT; }

// ==================== Light 方向计算 / Light Direction Calculation ====================

/**
 * @brief 获取指向光源的方向 / Get direction towards light
 */
vec3 getLightDir(Light light, vec3 worldPos)
{
    if (isDirectionalLight(light))
    {
        return normalize(-getLightDirection(light));
    }
    else
    {
        return normalize(getLightPosition(light) - worldPos);
    }
}

 // @brief 相机 Uniform Block（单例）/ Camera Uniform Block (singleton)
 layout(std140, binding = Camera_binding) uniform CameraBlock
 {
    Camera uCamera;
 };

 //* @brief 光源 Uniform Block（数组）/ Light Uniform Block (array)

 layout(std140, binding = Light_binding) uniform LightBlock
 {
    Light uLights[MAX_LIGHTS];
 };

#endif
