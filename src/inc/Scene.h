/**
 * @file Scene.h
 * @brief 场景类定义 / Scene class definition
 * 
 * 管理场景中的光源和可渲染对象，负责将光源数据上传到 GPU。
 * Manages lights and renderable objects in the scene, responsible for uploading light data to GPU.
 * 
 * 注意：相机由 View 管理，Scene 只负责光源。
 * Note: Camera is managed by View, Scene only manages lights.
 */

#ifndef SCENE_H
#define SCENE_H

#include "Light.h"
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

    void UpdateLightUBO();
    void BindLightUBO() const;
    int GetTotalLightCount() const;

private:
    void initUBOs();
    void convertDirectionalLight(const DirectionalLight& dirLight, glm::vec4* data);
    void convertPointLight(const PointLight& pointLight, glm::vec4* data);
    void convertSpotLight(const SpotLight& spotLight, glm::vec4* data);

private:
    LightManager m_lightManager;
    GLuint m_lightUBO;
    GLuint m_lightCountUBO;
};

#endif
