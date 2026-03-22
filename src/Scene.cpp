/**
 * @file Scene.cpp
 * @brief 场景类实现 / Scene class implementation
 */

#include "Scene.h"
#include "common/uniform_layout.h"
#include <cstring>

namespace {

struct GPULightData
{
    glm::vec4 colorIntensity;
    glm::vec4 positionInvRadius;
    glm::vec4 directionType;
    glm::vec4 params;
    GLint castShadows;
    GLint padding1;
    GLint padding2;
    GLint padding3;
};

}

Scene::Scene()
    : m_lightUBO(0)
    , m_lightCountUBO(0)
{
    initUBOs();
}

Scene::~Scene()
{
    if (m_lightUBO)
    {
        glDeleteBuffers(1, &m_lightUBO);
        m_lightUBO = 0;
    }
    if (m_lightCountUBO)
    {
        glDeleteBuffers(1, &m_lightCountUBO);
        m_lightCountUBO = 0;
    }
}

void Scene::initUBOs()
{
    glGenBuffers(1, &m_lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPULightData) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, Light_binding, m_lightUBO);

    glGenBuffers(1, &m_lightCountUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightCountUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLint), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, LightCount_binding, m_lightCountUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::UpdateLightUBO()
{
    GPULightData gpuData[MAX_LIGHTS];
    memset(gpuData, 0, sizeof(gpuData));

    int lightIndex = 0;

    const auto& dirLights = m_lightManager.GetDirectionalLights();
    for (const auto& dirLight : dirLights)
    {
        if (lightIndex >= MAX_LIGHTS) break;
        convertDirectionalLight(dirLight, reinterpret_cast<glm::vec4*>(&gpuData[lightIndex]));
        lightIndex++;
    }

    const auto& pointLights = m_lightManager.GetPointLights();
    for (const auto& pointLight : pointLights)
    {
        if (lightIndex >= MAX_LIGHTS) break;
        convertPointLight(pointLight, reinterpret_cast<glm::vec4*>(&gpuData[lightIndex]));
        lightIndex++;
    }

    const auto& spotLights = m_lightManager.GetSpotLights();
    for (const auto& spotLight : spotLights)
    {
        if (lightIndex >= MAX_LIGHTS) break;
        convertSpotLight(spotLight, reinterpret_cast<glm::vec4*>(&gpuData[lightIndex]));
        lightIndex++;
    }

    GLint totalLights = lightIndex;

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPULightData) * MAX_LIGHTS, gpuData);

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightCountUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint), &totalLights);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::BindLightUBO() const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, Light_binding, m_lightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, LightCount_binding, m_lightCountUBO);
}

int Scene::GetTotalLightCount() const
{
    return static_cast<int>(
        m_lightManager.GetDirectionalLightCount() +
        m_lightManager.GetPointLightCount() +
        m_lightManager.GetSpotLightCount()
    );
}

void Scene::convertDirectionalLight(const DirectionalLight& dirLight, glm::vec4* data)
{
    data[0] = glm::vec4(dirLight.color, dirLight.intensity);
    data[1] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    data[2] = glm::vec4(glm::normalize(dirLight.direction), 0.0f);
    data[3] = glm::vec4(0.0f, 0.0f, dirLight.colorTemperature, 0.0f);
    reinterpret_cast<GLint*>(data)[16] = dirLight.castShadows ? 1 : 0;
    reinterpret_cast<GLint*>(data)[17] = 0;
    reinterpret_cast<GLint*>(data)[18] = 0;
    reinterpret_cast<GLint*>(data)[19] = 0;
}

void Scene::convertPointLight(const PointLight& pointLight, glm::vec4* data)
{
    float invRadius = (pointLight.influenceRadius > 0.0f) ? (1.0f / pointLight.influenceRadius) : 0.0f;

    data[0] = glm::vec4(pointLight.color, pointLight.intensity);
    data[1] = glm::vec4(pointLight.position, invRadius);
    data[2] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    data[3] = glm::vec4(0.0f, 0.0f, pointLight.colorTemperature, pointLight.sourceRadius);
    reinterpret_cast<GLint*>(data)[16] = pointLight.castShadows ? 1 : 0;
    reinterpret_cast<GLint*>(data)[17] = 0;
    reinterpret_cast<GLint*>(data)[18] = 0;
    reinterpret_cast<GLint*>(data)[19] = 0;
}

void Scene::convertSpotLight(const SpotLight& spotLight, glm::vec4* data)
{
    float invRadius = (spotLight.influenceRadius > 0.0f) ? (1.0f / spotLight.influenceRadius) : 0.0f;

    data[0] = glm::vec4(spotLight.color, spotLight.intensity);
    data[1] = glm::vec4(spotLight.position, invRadius);
    data[2] = glm::vec4(glm::normalize(spotLight.direction), 2.0f);
    data[3] = glm::vec4(spotLight.innerConeAngle, spotLight.outerConeAngle, 
                        spotLight.colorTemperature, spotLight.sourceRadius);
    reinterpret_cast<GLint*>(data)[16] = spotLight.castShadows ? 1 : 0;
    reinterpret_cast<GLint*>(data)[17] = 0;
    reinterpret_cast<GLint*>(data)[18] = 0;
    reinterpret_cast<GLint*>(data)[19] = 0;
}
