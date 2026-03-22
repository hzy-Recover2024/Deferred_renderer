/**
 * @file Light.h
 * @brief 光源定义：平行光、点光源、聚光灯
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float colorTemperature;
    bool castShadows;

    DirectionalLight()
        : direction(0.0f, -1.0f, 0.0f), color(1.0f), intensity(1.0f)
        , colorTemperature(5500.0f), castShadows(false) {}

    DirectionalLight(const glm::vec3& dir, const glm::vec3& col, float inten)
        : direction(dir), color(col), intensity(inten)
        , colorTemperature(5500.0f), castShadows(false) {}
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float colorTemperature;
    bool castShadows;
    float sourceRadius;
    float influenceRadius;

    PointLight()
        : position(0.0f), color(1.0f), intensity(1.0f)
        , colorTemperature(4000.0f), castShadows(false)
        , sourceRadius(0.0f), influenceRadius(10.0f) {}

    PointLight(const glm::vec3& pos, const glm::vec3& col, float inten, float radius = 10.0f)
        : position(pos), color(col), intensity(inten)
        , colorTemperature(4000.0f), castShadows(false)
        , sourceRadius(0.0f), influenceRadius(radius) {}

    float CalculateAttenuation(float distance) const
    {
        float distRatio = distance / influenceRadius;
        return 1.0f / (1.0f + distRatio * distRatio);
    }
};

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float colorTemperature;
    bool castShadows;
    float sourceRadius;
    float innerConeAngle;
    float outerConeAngle;
    float influenceRadius;

    SpotLight()
        : position(0.0f), direction(0.0f, 0.0f, -1.0f), color(1.0f)
        , intensity(1.0f), colorTemperature(4000.0f), castShadows(false)
        , sourceRadius(0.0f)
        , innerConeAngle(glm::radians(12.5f)), outerConeAngle(glm::radians(17.5f))
        , influenceRadius(15.0f) {}

    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col,
              float inten, float inner, float outer)
        : position(pos), direction(dir), color(col), intensity(inten)
        , colorTemperature(4000.0f), castShadows(false), sourceRadius(0.0f)
        , innerConeAngle(glm::radians(inner)), outerConeAngle(glm::radians(outer))
        , influenceRadius(15.0f) {}

    float CalculateAttenuation(float distance) const
    {
        float distRatio = distance / influenceRadius;
        return 1.0f / (1.0f + distRatio * distRatio);
    }

    float CalculateConeAttenuation(const glm::vec3& lightDir) const
    {
        float theta = glm::dot(lightDir, glm::normalize(-direction));
        float innerCos = cos(innerConeAngle);
        float outerCos = cos(outerConeAngle);
        return glm::clamp((theta - outerCos) / (innerCos - outerCos), 0.0f, 1.0f);
    }
};

class LightManager
{
public:
    LightManager() = default;
    ~LightManager() = default;

    void AddDirectionalLight(const DirectionalLight& light);
    void AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
    void RemoveDirectionalLight(unsigned int index);
    void ClearDirectionalLights();
    size_t GetDirectionalLightCount() const { return m_directionalLights.size(); }
    DirectionalLight& GetDirectionalLight(unsigned int index);
    const DirectionalLight& GetDirectionalLight(unsigned int index) const;
    const std::vector<DirectionalLight>& GetDirectionalLights() const { return m_directionalLights; }
    std::vector<DirectionalLight>& GetDirectionalLights() { return m_directionalLights; }

    void AddPointLight(const PointLight& light);
    void AddPointLight(const glm::vec3& position, const glm::vec3& color,
                       float intensity, float influenceRadius = 10.0f);
    void RemovePointLight(unsigned int index);
    void ClearPointLights();
    size_t GetPointLightCount() const { return m_pointLights.size(); }
    PointLight& GetPointLight(unsigned int index);
    const PointLight& GetPointLight(unsigned int index) const;
    const std::vector<PointLight>& GetPointLights() const { return m_pointLights; }
    std::vector<PointLight>& GetPointLights() { return m_pointLights; }

    void AddSpotLight(const SpotLight& light);
    void AddSpotLight(const glm::vec3& position, const glm::vec3& direction,
                      const glm::vec3& color, float intensity,
                      float innerAngle = 12.5f, float outerAngle = 17.5f);
    void RemoveSpotLight(unsigned int index);
    void ClearSpotLights();
    size_t GetSpotLightCount() const { return m_spotLights.size(); }
    SpotLight& GetSpotLight(unsigned int index);
    const SpotLight& GetSpotLight(unsigned int index) const;
    const std::vector<SpotLight>& GetSpotLights() const { return m_spotLights; }
    std::vector<SpotLight>& GetSpotLights() { return m_spotLights; }

    void ClearAllLights();
    void GenerateRandomPointLights(unsigned int count,
                                   const glm::vec3& minBounds,
                                   const glm::vec3& maxBounds);

private:
    std::vector<DirectionalLight> m_directionalLights;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
};

#endif
