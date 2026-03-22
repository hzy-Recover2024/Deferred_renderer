/**
 * @file Light.h
 * @brief 光源定义 / Light definitions
 * 
 * 定义实时渲染中的直接光源：平行光、点光源、聚光灯。
 * Defines direct lights for real-time rendering: directional, point, and spot lights.
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

/**
 * @brief 平行光结构体 / Directional light structure
 * 
 * 模拟太阳光等无限远光源，无位置、无衰减。
 * Simulates infinite distance light sources like the sun, no position, no attenuation.
 * 
 * 使用示例 / Usage example:
 * @code
 * DirectionalLight sun;
 * sun.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
 * sun.color = glm::vec3(1.0f);
 * sun.intensity = 1.0f;
 * sun.colorTemperature = 5500.0f;  // 正午阳光 / Noon sunlight
 * sun.castShadows = true;
 * @endcode
 */
struct DirectionalLight
{
    glm::vec3 direction;            ///< 光照方向 / Light direction
    glm::vec3 color;                ///< 光源颜色 / Light color (RGB)
    float intensity;                ///< 光源强度 / Light intensity
    float colorTemperature;         ///< 色温（开尔文）/ Color temperature (Kelvin)
    bool castShadows;               ///< 是否投射阴影 / Whether to cast shadows

    /**
     * @brief 默认构造函数 / Default constructor
     */
    DirectionalLight()
        : direction(0.0f, -1.0f, 0.0f)
        , color(1.0f)
        , intensity(1.0f)
        , colorTemperature(5500.0f)
        , castShadows(false)
    {}

    /**
     * @brief 构造函数 / Constructor
     * @param dir 光照方向 / Light direction
     * @param col 光源颜色 / Light color
     * @param inten 光源强度 / Light intensity
     */
    DirectionalLight(const glm::vec3& dir, const glm::vec3& col, float inten)
        : direction(dir)
        , color(col)
        , intensity(inten)
        , colorTemperature(5500.0f)
        , castShadows(false)
    {}
};

/**
 * @brief 点光源结构体 / Point light structure
 * 
 * 从一点向所有方向发射光线的光源，有衰减。
 * Light source that emits light in all directions from a point, with attenuation.
 * 
 * 衰减公式 / Attenuation formula:
 * attenuation = 1 / (1 + (distance / influenceRadius)^2)
 * 
 * 使用示例 / Usage example:
 * @code
 * PointLight bulb;
 * bulb.position = glm::vec3(0.0f, 2.0f, 0.0f);
 * bulb.color = glm::vec3(1.0f, 0.9f, 0.8f);  // 暖色光 / Warm light
 * bulb.intensity = 10.0f;
 * bulb.sourceRadius = 0.1f;       // 小光源 / Small source
 * bulb.influenceRadius = 10.0f;   // 影响范围 / Influence range
 * @endcode
 */
struct PointLight
{
    glm::vec3 position;             ///< 光源位置 / Light position
    glm::vec3 color;                ///< 光源颜色 / Light color (RGB)
    float intensity;                ///< 光源强度 / Light intensity
    float colorTemperature;         ///< 色温（开尔文）/ Color temperature (Kelvin)
    bool castShadows;               ///< 是否投射阴影 / Whether to cast shadows

    float sourceRadius;             ///< 光源半径（用于软阴影）/ Source radius (for soft shadows)
    float influenceRadius;          ///< 影响半径（用于剔除优化）/ Influence radius (for culling optimization)

    /**
     * @brief 默认构造函数 / Default constructor
     */
    PointLight()
        : position(0.0f)
        , color(1.0f)
        , intensity(1.0f)
        , colorTemperature(4000.0f)
        , castShadows(false)
        , sourceRadius(0.0f)
        , influenceRadius(10.0f)
    {}

    /**
     * @brief 构造函数 / Constructor
     * @param pos 光源位置 / Light position
     * @param col 光源颜色 / Light color
     * @param inten 光源强度 / Light intensity
     * @param radius 影响半径 / Influence radius
     */
    PointLight(const glm::vec3& pos, const glm::vec3& col, float inten, float radius = 10.0f)
        : position(pos)
        , color(col)
        , intensity(inten)
        , colorTemperature(4000.0f)
        , castShadows(false)
        , sourceRadius(0.0f)
        , influenceRadius(radius)
    {}

    /**
     * @brief 计算衰减值 / Calculate attenuation
     * @param distance 距离 / Distance
     * @return float 衰减值 / Attenuation value
     */
    float CalculateAttenuation(float distance) const
    {
        float distRatio = distance / influenceRadius;
        return 1.0f / (1.0f + distRatio * distRatio);
    }
};

/**
 * @brief 聚光灯结构体 / Spot light structure
 * 
 * 从一点沿锥形方向发射光线的光源，有衰减和锥角限制。
 * Light source that emits light in a cone shape from a point, with attenuation and cone angle.
 * 
 * 锥角衰减公式 / Cone attenuation formula:
 * coneAttenuation = clamp((theta - outerAngle) / (innerAngle - outerAngle), 0, 1)
 * 
 * 使用示例 / Usage example:
 * @code
 * SpotLight flashlight;
 * flashlight.position = glm::vec3(0.0f, 0.0f, 0.0f);
 * flashlight.direction = glm::vec3(0.0f, 0.0f, -1.0f);
 * flashlight.color = glm::vec3(1.0f);
 * flashlight.intensity = 5.0f;
 * flashlight.innerConeAngle = glm::radians(12.5f);
 * flashlight.outerConeAngle = glm::radians(17.5f);
 * flashlight.influenceRadius = 20.0f;
 * @endcode
 */
struct SpotLight
{
    glm::vec3 position;             ///< 光源位置 / Light position
    glm::vec3 direction;            ///< 光照方向 / Light direction
    glm::vec3 color;                ///< 光源颜色 / Light color (RGB)
    float intensity;                ///< 光源强度 / Light intensity
    float colorTemperature;         ///< 色温（开尔文）/ Color temperature (Kelvin)
    bool castShadows;               ///< 是否投射阴影 / Whether to cast shadows

    float sourceRadius;             ///< 光源半径（用于软阴影）/ Source radius (for soft shadows)
    float innerConeAngle;           ///< 内锥角（弧度）/ Inner cone angle (radians)
    float outerConeAngle;           ///< 外锥角（弧度）/ Outer cone angle (radians)
    float influenceRadius;          ///< 影响半径（用于剔除优化）/ Influence radius (for culling optimization)

    /**
     * @brief 默认构造函数 / Default constructor
     */
    SpotLight()
        : position(0.0f)
        , direction(0.0f, 0.0f, -1.0f)
        , color(1.0f)
        , intensity(1.0f)
        , colorTemperature(4000.0f)
        , castShadows(false)
        , sourceRadius(0.0f)
        , innerConeAngle(glm::radians(12.5f))
        , outerConeAngle(glm::radians(17.5f))
        , influenceRadius(15.0f)
    {}

    /**
     * @brief 构造函数 / Constructor
     * @param pos 光源位置 / Light position
     * @param dir 光照方向 / Light direction
     * @param col 光源颜色 / Light color
     * @param inten 光源强度 / Light intensity
     * @param inner 内锥角（度数）/ Inner cone angle (degrees)
     * @param outer 外锥角（度数）/ Outer cone angle (degrees)
     */
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col, 
              float inten, float inner, float outer)
        : position(pos)
        , direction(dir)
        , color(col)
        , intensity(inten)
        , colorTemperature(4000.0f)
        , castShadows(false)
        , sourceRadius(0.0f)
        , innerConeAngle(glm::radians(inner))
        , outerConeAngle(glm::radians(outer))
        , influenceRadius(15.0f)
    {}

    /**
     * @brief 计算衰减值 / Calculate attenuation
     * @param distance 距离 / Distance
     * @return float 衰减值 / Attenuation value
     */
    float CalculateAttenuation(float distance) const
    {
        float distRatio = distance / influenceRadius;
        return 1.0f / (1.0f + distRatio * distRatio);
    }

    /**
     * @brief 计算锥角衰减 / Calculate cone attenuation
     * @param lightDir 光照方向（指向光源）/ Light direction (towards light)
     * @return float 锥角衰减值 / Cone attenuation value
     */
    float CalculateConeAttenuation(const glm::vec3& lightDir) const
    {
        float theta = glm::dot(lightDir, glm::normalize(-direction));
        float innerCos = cos(innerConeAngle);
        float outerCos = cos(outerConeAngle);
        return glm::clamp((theta - outerCos) / (innerCos - outerCos), 0.0f, 1.0f);
    }
};

/**
 * @brief 光源管理器 / Light manager
 * 
 * 管理场景中的所有光源。
 * Manages all lights in the scene.
 * 
 * 使用示例 / Usage example:
 * @code
 * LightManager lightManager;
 * 
 * // 添加平行光 / Add directional light
 * lightManager.AddDirectionalLight(DirectionalLight(...));
 * 
 * // 添加点光源 / Add point light
 * lightManager.AddPointLight(PointLight(...));
 * 
 * // 添加聚光灯 / Add spot light
 * lightManager.AddSpotLight(SpotLight(...));
 * @endcode
 */
class LightManager
{
public:
    /**
     * @brief 默认构造函数 / Default constructor
     */
    LightManager() = default;

    /**
     * @brief 析构函数 / Destructor
     */
    ~LightManager() = default;

    // ==================== 平行光操作 / Directional Light Operations ====================

    /**
     * @brief 添加平行光 / Add directional light
     * @param light 平行光 / Directional light
     */
    void AddDirectionalLight(const DirectionalLight& light);

    /**
     * @brief 添加平行光 / Add directional light
     * @param direction 光照方向 / Light direction
     * @param color 光源颜色 / Light color
     * @param intensity 光源强度 / Light intensity
     */
    void AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

    /**
     * @brief 移除平行光 / Remove directional light
     * @param index 索引 / Index
     */
    void RemoveDirectionalLight(unsigned int index);

    /**
     * @brief 清除所有平行光 / Clear all directional lights
     */
    void ClearDirectionalLights();

    /**
     * @brief 获取平行光数量 / Get directional light count
     * @return size_t 数量 / Count
     */
    size_t GetDirectionalLightCount() const { return m_directionalLights.size(); }

    /**
     * @brief 获取平行光 / Get directional light
     * @param index 索引 / Index
     * @return DirectionalLight& 平行光引用 / Directional light reference
     */
    DirectionalLight& GetDirectionalLight(unsigned int index);
    const DirectionalLight& GetDirectionalLight(unsigned int index) const;

    /**
     * @brief 获取所有平行光 / Get all directional lights
     * @return const std::vector<DirectionalLight>& 平行光数组引用 / Directional lights reference
     */
    const std::vector<DirectionalLight>& GetDirectionalLights() const { return m_directionalLights; }
    std::vector<DirectionalLight>& GetDirectionalLights() { return m_directionalLights; }

    // ==================== 点光源操作 / Point Light Operations ====================

    /**
     * @brief 添加点光源 / Add point light
     * @param light 点光源 / Point light
     */
    void AddPointLight(const PointLight& light);

    /**
     * @brief 添加点光源 / Add point light
     * @param position 光源位置 / Light position
     * @param color 光源颜色 / Light color
     * @param intensity 光源强度 / Light intensity
     * @param influenceRadius 影响半径 / Influence radius
     */
    void AddPointLight(const glm::vec3& position, const glm::vec3& color, 
                       float intensity, float influenceRadius = 10.0f);

    /**
     * @brief 移除点光源 / Remove point light
     * @param index 索引 / Index
     */
    void RemovePointLight(unsigned int index);

    /**
     * @brief 清除所有点光源 / Clear all point lights
     */
    void ClearPointLights();

    /**
     * @brief 获取点光源数量 / Get point light count
     * @return size_t 数量 / Count
     */
    size_t GetPointLightCount() const { return m_pointLights.size(); }

    /**
     * @brief 获取点光源 / Get point light
     * @param index 索引 / Index
     * @return PointLight& 点光源引用 / Point light reference
     */
    PointLight& GetPointLight(unsigned int index);
    const PointLight& GetPointLight(unsigned int index) const;

    /**
     * @brief 获取所有点光源 / Get all point lights
     * @return const std::vector<PointLight>& 点光源数组引用 / Point lights reference
     */
    const std::vector<PointLight>& GetPointLights() const { return m_pointLights; }
    std::vector<PointLight>& GetPointLights() { return m_pointLights; }

    // ==================== 聚光灯操作 / Spot Light Operations ====================

    /**
     * @brief 添加聚光灯 / Add spot light
     * @param light 聚光灯 / Spot light
     */
    void AddSpotLight(const SpotLight& light);

    /**
     * @brief 添加聚光灯 / Add spot light
     * @param position 光源位置 / Light position
     * @param direction 光照方向 / Light direction
     * @param color 光源颜色 / Light color
     * @param intensity 光源强度 / Light intensity
     * @param innerAngle 内锥角（度数）/ Inner cone angle (degrees)
     * @param outerAngle 外锥角（度数）/ Outer cone angle (degrees)
     */
    void AddSpotLight(const glm::vec3& position, const glm::vec3& direction, 
                      const glm::vec3& color, float intensity,
                      float innerAngle = 12.5f, float outerAngle = 17.5f);

    /**
     * @brief 移除聚光灯 / Remove spot light
     * @param index 索引 / Index
     */
    void RemoveSpotLight(unsigned int index);

    /**
     * @brief 清除所有聚光灯 / Clear all spot lights
     */
    void ClearSpotLights();

    /**
     * @brief 获取聚光灯数量 / Get spot light count
     * @return size_t 数量 / Count
     */
    size_t GetSpotLightCount() const { return m_spotLights.size(); }

    /**
     * @brief 获取聚光灯 / Get spot light
     * @param index 索引 / Index
     * @return SpotLight& 聚光灯引用 / Spot light reference
     */
    SpotLight& GetSpotLight(unsigned int index);
    const SpotLight& GetSpotLight(unsigned int index) const;

    /**
     * @brief 获取所有聚光灯 / Get all spot lights
     * @return const std::vector<SpotLight>& 聚光灯数组引用 / Spot lights reference
     */
    const std::vector<SpotLight>& GetSpotLights() const { return m_spotLights; }
    std::vector<SpotLight>& GetSpotLights() { return m_spotLights; }

    // ==================== 通用操作 / General Operations ====================

    /**
     * @brief 清除所有光源 / Clear all lights
     */
    void ClearAllLights();

    /**
     * @brief 生成随机点光源 / Generate random point lights
     * @param count 数量 / Count
     * @param minBounds 最小边界 / Minimum bounds
     * @param maxBounds 最大边界 / Maximum bounds
     */
    void GenerateRandomPointLights(unsigned int count, 
                                   const glm::vec3& minBounds, 
                                   const glm::vec3& maxBounds);

private:
    std::vector<DirectionalLight> m_directionalLights;   ///< 平行光列表 / Directional lights
    std::vector<PointLight> m_pointLights;               ///< 点光源列表 / Point lights
    std::vector<SpotLight> m_spotLights;                 ///< 聚光灯列表 / Spot lights
};

#endif
