#include "deferred/DeferredRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DeferredRenderer::DeferredRenderer()
    : m_GeometryShader(nullptr), m_LightingShader(nullptr), m_LightBoxShader(nullptr),
      m_Width(0), m_Height(0), m_CubeVAO(0), m_CubeVBO(0), m_CubeInitialized(false)
{
}

bool DeferredRenderer::Init(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;

    m_GBuffer = std::make_shared<GBuffer>();
    if (!m_GBuffer->Init(width, height))
    {
        std::cerr << "Failed to initialize GBuffer!" << std::endl;
        return false;
    }

    return true;
}

void DeferredRenderer::Resize(unsigned int width, unsigned int height)
{
    if (m_Width == width && m_Height == height)
        return;

    m_Width = width;
    m_Height = height;

    if (m_GBuffer)
    {
        m_GBuffer->Resize(width, height);
    }
}

void DeferredRenderer::BeginGeometryPass(const glm::mat4& projection, const glm::mat4& view)
{
    m_GBuffer->BindForWriting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_GeometryShader)
    {
        m_GeometryShader->use();
        m_GeometryShader->setMat4("projection", projection);
        m_GeometryShader->setMat4("view", view);
    }
}

void DeferredRenderer::EndGeometryPass()
{
    m_GBuffer->Unbind();
}

void DeferredRenderer::RenderLightingPass(const glm::vec3& cameraPos)
{
    m_GBuffer->BindForReading();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_LightingShader)
    {
        m_LightingShader->use();

        m_GBuffer->BindTexture(GBuffer::POSITION, 0);
        m_LightingShader->setInt("gPosition", 0);

        m_GBuffer->BindTexture(GBuffer::NORMAL, 1);
        m_LightingShader->setInt("gNormal", 1);

        m_GBuffer->BindTexture(GBuffer::ALBEDO_SPEC, 2);
        m_LightingShader->setInt("gAlbedoSpec", 2);

        m_LightingShader->setVec3("viewPos", cameraPos);

        const auto& lights = m_LightManager.GetPointLights();
        for (size_t i = 0; i < lights.size(); ++i)
        {
            std::string prefix = "lights[" + std::to_string(i) + "]";
            m_LightingShader->setVec3(prefix + ".Position", lights[i].Position);
            m_LightingShader->setVec3(prefix + ".Color", lights[i].Color);
            m_LightingShader->setFloat(prefix + ".Linear", lights[i].Linear);
            m_LightingShader->setFloat(prefix + ".Quadratic", lights[i].Quadratic);
        }

        RenderQuad();
    }
}

void DeferredRenderer::CopyDepthBuffer()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->GetFramebuffer()->FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::RenderLightBoxes(const glm::mat4& projection, const glm::mat4& view)
{
    if (!m_LightBoxShader)
        return;

    m_LightBoxShader->use();
    m_LightBoxShader->setMat4("projection", projection);
    m_LightBoxShader->setMat4("view", view);

    const auto& lights = m_LightManager.GetPointLights();
    for (const auto& light : lights)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light.Position);
        model = glm::scale(model, glm::vec3(0.125f));
        m_LightBoxShader->setMat4("model", model);
        m_LightBoxShader->setVec3("lightColor", light.Color);
        RenderCube();
    }
}

void DeferredRenderer::InitCube()
{
    if (m_CubeInitialized)
        return;

    float vertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
    };

    glGenVertexArrays(1, &m_CubeVAO);
    glGenBuffers(1, &m_CubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_CubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    m_CubeInitialized = true;
}

void DeferredRenderer::RenderCube()
{
    if (!m_CubeInitialized)
        InitCube();

    glBindVertexArray(m_CubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void DeferredRenderer::RenderQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO = 0;
    static bool quadInitialized = false;

    if (!quadInitialized)
    {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
        quadInitialized = true;
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
