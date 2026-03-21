#include "deferred/Light.h"
#include <stdexcept>
#include <cstdlib>
#include <ctime>

void LightManager::AddPointLight(const PointLight& light)
{
    m_PointLights.push_back(light);
}

void LightManager::AddPointLight(const glm::vec3& position, const glm::vec3& color, float linear, float quadratic)
{
    m_PointLights.emplace_back(position, color, linear, quadratic);
}

void LightManager::RemovePointLight(unsigned int index)
{
    if (index < m_PointLights.size())
    {
        m_PointLights.erase(m_PointLights.begin() + index);
    }
}

void LightManager::ClearLights()
{
    m_PointLights.clear();
}

PointLight& LightManager::GetPointLight(unsigned int index)
{
    return m_PointLights.at(index);
}

const PointLight& LightManager::GetPointLight(unsigned int index) const
{
    return m_PointLights.at(index);
}

void LightManager::GenerateRandomLights(unsigned int count, const glm::vec3& minBounds, const glm::vec3& maxBounds)
{
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    m_PointLights.clear();
    m_PointLights.reserve(count);

    for (unsigned int i = 0; i < count; ++i)
    {
        float x = minBounds.x + (static_cast<float>(std::rand() % 100) / 100.0f) * (maxBounds.x - minBounds.x);
        float y = minBounds.y + (static_cast<float>(std::rand() % 100) / 100.0f) * (maxBounds.y - minBounds.y);
        float z = minBounds.z + (static_cast<float>(std::rand() % 100) / 100.0f) * (maxBounds.z - minBounds.z);

        float r = 0.5f + (static_cast<float>(std::rand() % 100) / 200.0f);
        float g = 0.5f + (static_cast<float>(std::rand() % 100) / 200.0f);
        float b = 0.5f + (static_cast<float>(std::rand() % 100) / 200.0f);

        AddPointLight(glm::vec3(x, y, z), glm::vec3(r, g, b));
    }
}
