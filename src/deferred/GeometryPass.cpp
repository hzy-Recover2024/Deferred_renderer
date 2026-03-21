#include "deferred/GeometryPass.h"

GeometryPass::GeometryPass(const std::string& name, std::shared_ptr<GBuffer> gbuffer)
    : RenderPass(name), m_GBuffer(gbuffer), m_Shader(nullptr), m_Projection(1.0f), m_View(1.0f)
{
}

void GeometryPass::Setup()
{
    if (m_GBuffer)
    {
        AddOutput(m_GBuffer->GetFramebuffer());
    }
}

void GeometryPass::Execute()
{
    if (!m_GBuffer || !m_Shader)
        return;

    m_GBuffer->BindForWriting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Shader->use();
    m_Shader->setMat4("projection", m_Projection);
    m_Shader->setMat4("view", m_View);
}
