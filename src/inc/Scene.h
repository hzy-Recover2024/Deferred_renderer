/**
 * @file Scene.h
 * @brief 场景类，管理光源数据并上传到 GPU
 */

#ifndef SCENE_H
#define SCENE_H

#include "Light.h"
#include "UniformBuffer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Scene
{
public:
    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    LightManager& GetLightManager() { return m_lightManager; }
    const LightManager& GetLightManager() const { return m_lightManager; }

    void MarkDirty() { m_Dirty = true; }
    void UpdateLightUBO();
    void BindLightUBO() const;
    int GetTotalLightCount() const;

private:
    void initUBO();

private:
    LightManager m_lightManager;
    RefPtr<UniformBuffer> m_LightUBO;
    bool m_Dirty = true;
};

#endif
