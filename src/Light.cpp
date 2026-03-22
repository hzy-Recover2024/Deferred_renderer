/**
 * @file Light.cpp
 * @brief 光源管理器实现 / Light manager implementation
 */

#include "Light.h"
#include <stdexcept>
#include <random>

// ==================== 平行光操作 / Directional Light Operations ====================

void LightManager::AddDirectionalLight(const DirectionalLight& light)
{
    m_directionalLights.push_back(light);
}

void LightManager::AddDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
{
    m_directionalLights.emplace_back(direction, color, intensity);
}

void LightManager::RemoveDirectionalLight(unsigned int index)
{
    if (index >= m_directionalLights.size())
    {
        throw std::out_of_range("Directional light index out of range");
    }
    m_directionalLights.erase(m_directionalLights.begin() + index);
}

void LightManager::ClearDirectionalLights()
{
    m_directionalLights.clear();
}

DirectionalLight& LightManager::GetDirectionalLight(unsigned int index)
{
    if (index >= m_directionalLights.size())
    {
        throw std::out_of_range("Directional light index out of range");
    }
    return m_directionalLights[index];
}

const DirectionalLight& LightManager::GetDirectionalLight(unsigned int index) const
{
    if (index >= m_directionalLights.size())
    {
        throw std::out_of_range("Directional light index out of range");
    }
    return m_directionalLights[index];
}

// ==================== 点光源操作 / Point Light Operations ====================

void LightManager::AddPointLight(const PointLight& light)
{
    m_pointLights.push_back(light);
}

void LightManager::AddPointLight(const glm::vec3& position, const glm::vec3& color, 
                                  float intensity, float influenceRadius)
{
    m_pointLights.emplace_back(position, color, intensity, influenceRadius);
}

void LightManager::RemovePointLight(unsigned int index)
{
    if (index >= m_pointLights.size())
    {
        throw std::out_of_range("Point light index out of range");
    }
    m_pointLights.erase(m_pointLights.begin() + index);
}

void LightManager::ClearPointLights()
{
    m_pointLights.clear();
}

PointLight& LightManager::GetPointLight(unsigned int index)
{
    if (index >= m_pointLights.size())
    {
        throw std::out_of_range("Point light index out of range");
    }
    return m_pointLights[index];
}

const PointLight& LightManager::GetPointLight(unsigned int index) const
{
    if (index >= m_pointLights.size())
    {
        throw std::out_of_range("Point light index out of range");
    }
    return m_pointLights[index];
}

// ==================== 聚光灯操作 / Spot Light Operations ====================

void LightManager::AddSpotLight(const SpotLight& light)
{
    m_spotLights.push_back(light);
}

void LightManager::AddSpotLight(const glm::vec3& position, const glm::vec3& direction, 
                                 const glm::vec3& color, float intensity,
                                 float innerAngle, float outerAngle)
{
    m_spotLights.emplace_back(position, direction, color, intensity, innerAngle, outerAngle);
}

void LightManager::RemoveSpotLight(unsigned int index)
{
    if (index >= m_spotLights.size())
    {
        throw std::out_of_range("Spot light index out of range");
    }
    m_spotLights.erase(m_spotLights.begin() + index);
}

void LightManager::ClearSpotLights()
{
    m_spotLights.clear();
}

SpotLight& LightManager::GetSpotLight(unsigned int index)
{
    if (index >= m_spotLights.size())
    {
        throw std::out_of_range("Spot light index out of range");
    }
    return m_spotLights[index];
}

const SpotLight& LightManager::GetSpotLight(unsigned int index) const
{
    if (index >= m_spotLights.size())
    {
        throw std::out_of_range("Spot light index out of range");
    }
    return m_spotLights[index];
}

// ==================== 通用操作 / General Operations ====================

void LightManager::ClearAllLights()
{
    m_directionalLights.clear();
    m_pointLights.clear();
    m_spotLights.clear();
}

void LightManager::GenerateRandomPointLights(unsigned int count, 
                                              const glm::vec3& minBounds, 
                                              const glm::vec3& maxBounds)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> posX(minBounds.x, maxBounds.x);
    std::uniform_real_distribution<float> posY(minBounds.y, maxBounds.y);
    std::uniform_real_distribution<float> posZ(minBounds.z, maxBounds.z);
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> intensityDist(5.0f, 20.0f);
    std::uniform_real_distribution<float> radiusDist(5.0f, 15.0f);

    for (unsigned int i = 0; i < count; ++i)
    {
        PointLight light;
        light.position = glm::vec3(posX(gen), posY(gen), posZ(gen));
        light.color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
        light.intensity = intensityDist(gen);
        light.influenceRadius = radiusDist(gen);
        m_pointLights.push_back(light);
    }
}
