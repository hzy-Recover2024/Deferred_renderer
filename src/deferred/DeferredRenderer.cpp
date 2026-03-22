/**
 * @file DeferredRenderer.cpp
 * @brief 延迟渲染器实现
 */

#include "deferred/DeferredRenderer.h"
#include <iostream>

DeferredRenderer::DeferredRenderer()
    : m_GeometryPass(nullptr), m_LightingPass(nullptr),
      m_GeometryShader(nullptr), m_LightingShader(nullptr),
      m_Projection(1.0f), m_View(1.0f), m_CameraPosition(0.0f),
      m_Width(0), m_Height(0), m_RenderTargetFBO(0), m_Initialized(false)
{
}

bool DeferredRenderer::Init(unsigned int width, unsigned int height, const std::string& shaderPath)
{
    if (m_Initialized)
    {
        std::cerr << "DeferredRenderer: Already initialized!" << std::endl;
        return false;
    }

    m_Width = width;
    m_Height = height;

    m_GBuffer = std::make_shared<GBuffer>();
    if (!m_GBuffer->Init(width, height))
    {
        std::cerr << "DeferredRenderer: Failed to initialize G-Buffer!" << std::endl;
        return false;
    }

    if (!LoadShaders(shaderPath))
    {
        std::cerr << "DeferredRenderer: Failed to load shaders!" << std::endl;
        return false;
    }

    AssembleRenderGraph();

    m_Initialized = true;
    return true;
}

bool DeferredRenderer::LoadShaders(const std::string& shaderPath)
{
    auto& shaderManager = ShaderManager::GetInstance();

    m_GeometryShader = shaderManager.Load(
        "deferred_geometry",
        shaderPath + "/deferred/gbuffer.vs",
        shaderPath + "/deferred/gbuffer.fs"
    );
    if (!m_GeometryShader)
    {
        std::cerr << "DeferredRenderer: Failed to load geometry shader!" << std::endl;
        return false;
    }

    m_LightingShader = shaderManager.Load(
        "deferred_lighting",
        shaderPath + "/deferred/deferred_shading.vs",
        shaderPath + "/deferred/deferred_shading.fs"
    );
    if (!m_LightingShader)
    {
        std::cerr << "DeferredRenderer: Failed to load lighting shader!" << std::endl;
        return false;
    }

    return true;
}

void DeferredRenderer::AssembleRenderGraph()
{
    auto geometryPass = m_RenderGraph.CreatePass<GeometryPass>("GeometryPass", m_GBuffer);
    geometryPass->SetShader(m_GeometryShader);
    m_GeometryPass = geometryPass.get();

    auto lightingPass = m_RenderGraph.CreatePass<LightingPass>("LightingPass", m_GBuffer);
    lightingPass->SetShader(m_LightingShader);
    lightingPass->SetLightManager(&m_LightManager);
    lightingPass->SetScreenSize(m_Width, m_Height);
    m_LightingPass = lightingPass.get();

    lightingPass->AddDependency(geometryPass);
}

void DeferredRenderer::SetCamera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position)
{
    m_Projection = projection;
    m_View = view;
    m_CameraPosition = position;

    if (m_GeometryPass)
    {
        m_GeometryPass->SetProjection(m_Projection);
        m_GeometryPass->SetView(m_View);
    }

    if (m_LightingPass)
    {
        m_LightingPass->SetCameraPosition(m_CameraPosition);
    }
}

void DeferredRenderer::SetRenderTarget(unsigned int fbo)
{
    m_RenderTargetFBO = fbo;

    if (m_LightingPass)
    {
        m_LightingPass->SetRenderTarget(fbo);
    }
}

void DeferredRenderer::Resize(unsigned int width, unsigned int height)
{
    if (!m_Initialized)
        return;

    if (m_Width == width && m_Height == height)
        return;

    m_Width = width;
    m_Height = height;

    if (m_GBuffer)
    {
        m_GBuffer->Resize(width, height);
    }

    if (m_LightingPass)
    {
        m_LightingPass->SetScreenSize(width, height);
    }

    m_RenderGraph.SetDirty();
}

void DeferredRenderer::Render()
{
    if (!m_Initialized)
    {
        std::cerr << "DeferredRenderer: Not initialized!" << std::endl;
        return;
    }

    m_RenderGraph.Compile();
    m_RenderGraph.Execute();
}

void DeferredRenderer::SetSceneRenderCallback(SceneRenderCallback callback)
{
    if (m_GeometryPass)
    {
        m_GeometryPass->SetRenderCallback(callback);
    }
}
