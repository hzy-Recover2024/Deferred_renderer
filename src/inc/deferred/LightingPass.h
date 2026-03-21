#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H

#include "rendergraph/RenderPass.h"
#include "GBuffer.h"
#include "Light.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>
#include <memory>

class LightingPass : public RenderPass
{
public:
    LightingPass(const std::string& name, std::shared_ptr<GBuffer> gbuffer);
    ~LightingPass() = default;

    void Setup() override;
    void Execute() override;

    void SetShader(Shader* shader) { m_Shader = shader; }
    void SetLightManager(LightManager* manager) { m_LightManager = manager; }
    void SetCameraPosition(const glm::vec3& pos) { m_CameraPosition = pos; }

    void RenderQuad();

private:
    std::shared_ptr<GBuffer> m_GBuffer;
    Shader* m_Shader;
    LightManager* m_LightManager;
    glm::vec3 m_CameraPosition;

    unsigned int m_QuadVAO;
    unsigned int m_QuadVBO;
    bool m_QuadInitialized;

    void InitQuad();
};

#endif
