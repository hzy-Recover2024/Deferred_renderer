#include "deferred/LightingPass.h"
#include <iostream>

LightingPass::LightingPass(const std::string& name, std::shared_ptr<GBuffer> gbuffer)
    : RenderPass(name), m_GBuffer(gbuffer), m_Shader(nullptr), m_LightManager(nullptr),
      m_CameraPosition(0.0f), m_QuadVAO(0), m_QuadVBO(0), m_QuadInitialized(false)
{
}

void LightingPass::Setup()
{
    if (m_GBuffer)
    {
        AddInput(m_GBuffer->GetFramebuffer());
    }
}

void LightingPass::Execute()
{
    if (!m_GBuffer || !m_Shader)
        return;

    m_GBuffer->BindForReading();

    m_Shader->use();

    m_GBuffer->BindTexture(GBuffer::POSITION, 0);
    m_Shader->setInt("gPosition", 0);

    m_GBuffer->BindTexture(GBuffer::NORMAL, 1);
    m_Shader->setInt("gNormal", 1);

    m_GBuffer->BindTexture(GBuffer::ALBEDO_SPEC, 2);
    m_Shader->setInt("gAlbedoSpec", 2);

    m_Shader->setVec3("viewPos", m_CameraPosition);

    if (m_LightManager)
    {
        const auto& lights = m_LightManager->GetPointLights();
        for (size_t i = 0; i < lights.size(); ++i)
        {
            std::string prefix = "lights[" + std::to_string(i) + "]";
            m_Shader->setVec3(prefix + ".Position", lights[i].Position);
            m_Shader->setVec3(prefix + ".Color", lights[i].Color);
            m_Shader->setFloat(prefix + ".Linear", lights[i].Linear);
            m_Shader->setFloat(prefix + ".Quadratic", lights[i].Quadratic);
        }
    }

    RenderQuad();
}

void LightingPass::InitQuad()
{
    if (m_QuadInitialized)
        return;

    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);

    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    m_QuadInitialized = true;
}

void LightingPass::RenderQuad()
{
    if (!m_QuadInitialized)
        InitQuad();

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
