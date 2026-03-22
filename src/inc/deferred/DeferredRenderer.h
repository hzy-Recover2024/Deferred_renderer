/**
 * @file DeferredRenderer.h
 * @brief 延迟渲染器
 */

#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "RenderGraph.h"
#include "ShaderManager.h"
#include "Framebuffer.h"
#include "GBuffer.h"
#include "GeometryPass.h"
#include "LightingPass.h"
#include "Light.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>
#include <memory>
#include <functional>

class DeferredRenderer
{
public:
    using SceneRenderCallback = std::function<void(Shader* shader)>;

    DeferredRenderer();
    ~DeferredRenderer() = default;

    bool Init(unsigned int width, unsigned int height, const std::string& shaderPath = "shader");

    void SetCamera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position);
    void SetRenderTarget(unsigned int fbo);
    void Resize(unsigned int width, unsigned int height);
    void Render();

    void SetSceneRenderCallback(SceneRenderCallback callback);

    LightManager& GetLightManager() { return m_LightManager; }
    const LightManager& GetLightManager() const { return m_LightManager; }

    RefPtr<GBuffer> GetGBuffer() const { return m_GBuffer; }
    RenderGraph& GetRenderGraph() { return m_RenderGraph; }
    const RenderGraph& GetRenderGraph() const { return m_RenderGraph; }

    RefPtr<Shader> GetGeometryShader() const { return m_GeometryShader; }
    RefPtr<Shader> GetLightingShader() const { return m_LightingShader; }

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }
    unsigned int GetRenderTarget() const { return m_RenderTargetFBO; }
    bool IsInitialized() const { return m_Initialized; }

private:
    RenderGraph m_RenderGraph;
    RefPtr<GBuffer> m_GBuffer;

    GeometryPass* m_GeometryPass;
    LightingPass* m_LightingPass;

    LightManager m_LightManager;

    RefPtr<Shader> m_GeometryShader;
    RefPtr<Shader> m_LightingShader;

    glm::mat4 m_Projection;
    glm::mat4 m_View;
    glm::vec3 m_CameraPosition;

    unsigned int m_Width;
    unsigned int m_Height;
    unsigned int m_RenderTargetFBO;
    bool m_Initialized;

    bool LoadShaders(const std::string& shaderPath);
    void AssembleRenderGraph();
};

#endif
