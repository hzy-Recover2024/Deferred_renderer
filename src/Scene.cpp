/**
 * @file Scene.cpp
 * @brief 场景类实现
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

static_assert(sizeof(GPULightData) == 80, "GPULightData size must be 80 bytes for std140 layout");

void convertDirectionalLight(const DirectionalLight& dirLight, GPULightData* data)
{
    data->colorIntensity = glm::vec4(dirLight.color, dirLight.intensity);
    data->positionInvRadius = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    data->directionType = glm::vec4(glm::normalize(dirLight.direction), 0.0f);
    data->params = glm::vec4(0.0f, 0.0f, dirLight.colorTemperature, 0.0f);
    data->castShadows = dirLight.castShadows ? 1 : 0;
    data->padding1 = 0;
    data->padding2 = 0;
    data->padding3 = 0;
}

void convertPointLight(const PointLight& pointLight, GPULightData* data)
{
    float invRadius = (pointLight.influenceRadius > 0.0f) ? (1.0f / pointLight.influenceRadius) : 0.0f;

    data->colorIntensity = glm::vec4(pointLight.color, pointLight.intensity);
    data->positionInvRadius = glm::vec4(pointLight.position, invRadius);
    data->directionType = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    data->params = glm::vec4(0.0f, 0.0f, pointLight.colorTemperature, pointLight.sourceRadius);
    data->castShadows = pointLight.castShadows ? 1 : 0;
    data->padding1 = 0;
    data->padding2 = 0;
    data->padding3 = 0;
}

void convertSpotLight(const SpotLight& spotLight, GPULightData* data)
{
    float invRadius = (spotLight.influenceRadius > 0.0f) ? (1.0f / spotLight.influenceRadius) : 0.0f;

    data->colorIntensity = glm::vec4(spotLight.color, spotLight.intensity);
    data->positionInvRadius = glm::vec4(spotLight.position, invRadius);
    data->directionType = glm::vec4(glm::normalize(spotLight.direction), 2.0f);
    data->params = glm::vec4(spotLight.innerConeAngle, spotLight.outerConeAngle,
                             spotLight.colorTemperature, spotLight.sourceRadius);
    data->castShadows = spotLight.castShadows ? 1 : 0;
    data->padding1 = 0;
    data->padding2 = 0;
    data->padding3 = 0;
}

}

Scene::Scene()
    : m_LightUBO(nullptr)
    , m_Dirty(true)
{
    initUBO();
}

Scene::~Scene() = default;

void Scene::initUBO()
{
    size_t bufferSize = 16 + sizeof(GPULightData) * MAX_LIGHTS;
    m_LightUBO = MakeRefPtr<UniformBuffer>("LightUBO", bufferSize, Light_binding);
    m_LightUBO->Create();
}

void Scene::UpdateLightUBO()
{
    if (!m_Dirty || !m_LightUBO)
        return;

    size_t bufferSize = 16 + sizeof(GPULightData) * MAX_LIGHTS;
    std::vector<char> buffer(bufferSize, 0);

    GLint lightCount = 0;
    GPULightData* lightData = reinterpret_cast<GPULightData*>(buffer.data() + 16);

    for (const auto& dirLight : m_lightManager.GetDirectionalLights())
    {
        if (lightCount >= MAX_LIGHTS) break;
        convertDirectionalLight(dirLight, lightData + lightCount);
        lightCount++;
    }

    for (const auto& pointLight : m_lightManager.GetPointLights())
    {
        if (lightCount >= MAX_LIGHTS) break;
        convertPointLight(pointLight, lightData + lightCount);
        lightCount++;
    }

    for (const auto& spotLight : m_lightManager.GetSpotLights())
    {
        if (lightCount >= MAX_LIGHTS) break;
        convertSpotLight(spotLight, lightData + lightCount);
        lightCount++;
    }

    memcpy(buffer.data(), &lightCount, sizeof(GLint));
    m_LightUBO->SetData(buffer.data(), bufferSize);
    m_Dirty = false;
}

void Scene::BindLightUBO() const
{
    if (m_LightUBO)
        m_LightUBO->Bind();
}

int Scene::GetTotalLightCount() const
{
    return static_cast<int>(
        m_lightManager.GetDirectionalLightCount() +
        m_lightManager.GetPointLightCount() +
        m_lightManager.GetSpotLightCount()
    );
}
