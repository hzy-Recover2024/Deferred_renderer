#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;

    PointLight()
        : Position(0.0f), Color(1.0f), Linear(0.7f), Quadratic(1.8f), Radius(10.0f)
    {
    }

    PointLight(const glm::vec3& pos, const glm::vec3& color, float linear = 0.7f, float quadratic = 1.8f)
        : Position(pos), Color(color), Linear(linear), Quadratic(quadratic), Radius(10.0f)
    {
        CalculateRadius();
    }

    void CalculateRadius(float threshold = 256.0f / 5.0f)
    {
        float maxBrightness = std::fmaxf(std::fmaxf(Color.r, Color.g), Color.b);
        Radius = (-Linear + std::sqrt(Linear * Linear - 4 * Quadratic * (1.0f - threshold * maxBrightness))) / (2.0f * Quadratic);
    }
};

class LightManager
{
public:
    LightManager() = default;
    ~LightManager() = default;

    void AddPointLight(const PointLight& light);
    void AddPointLight(const glm::vec3& position, const glm::vec3& color, float linear = 0.7f, float quadratic = 1.8f);
    void RemovePointLight(unsigned int index);
    void ClearLights();

    PointLight& GetPointLight(unsigned int index);
    const PointLight& GetPointLight(unsigned int index) const;
    
    size_t GetPointLightCount() const { return m_PointLights.size(); }
    const std::vector<PointLight>& GetPointLights() const { return m_PointLights; }
    std::vector<PointLight>& GetPointLights() { return m_PointLights; }

    void GenerateRandomLights(unsigned int count, const glm::vec3& minBounds, const glm::vec3& maxBounds);

private:
    std::vector<PointLight> m_PointLights;
};

#endif
