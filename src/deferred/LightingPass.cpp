/**
 * @file LightingPass.cpp
 * @brief 光照通道实现
 */

#include "deferred/LightingPass.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

LightingPass::LightingPass(const std::string& name, RefPtr<GBuffer> gbuffer)
    : RenderPass(name), m_GBuffer(gbuffer), m_Shader(nullptr), m_LightManager(nullptr),
      m_CameraPosition(0.0f), m_RenderTargetFBO(0), m_Width(800), m_Height(600)
{
}

void LightingPass::Setup()
{
}

void LightingPass::Execute()
{
    if (!m_GBuffer || !m_Shader)
        return;

    unsigned int gbufferFBO = m_GBuffer->GetFramebuffer()->FBO;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbufferFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RenderTargetFBO);
    glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTargetFBO);

    m_GBuffer->BindForReading();

    m_Shader->use();

    m_Shader->setInt("gPosition", 0);
    m_Shader->setInt("gNormal", 1);
    m_Shader->setInt("gAlbedoSpec", 2);

    m_Shader->setVec3("viewPos", m_CameraPosition);

    ScreenQuad::GetInstance().Render();
}
