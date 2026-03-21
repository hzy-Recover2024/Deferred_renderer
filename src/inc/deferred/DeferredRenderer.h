#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "GBuffer.h"
#include "Light.h"
#include "GeometryPass.h"
#include "LightingPass.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>
#include <memory>

class DeferredRenderer
{
public:
    DeferredRenderer();
    ~DeferredRenderer() = default;

    bool Init(unsigned int width, unsigned int height);
    void Resize(unsigned int width, unsigned int height);

    void BeginGeometryPass(const glm::mat4& projection, const glm::mat4& view);
    void EndGeometryPass();
    void RenderLightingPass(const glm::vec3& cameraPos);
    void CopyDepthBuffer();
    void RenderLightBoxes(const glm::mat4& projection, const glm::mat4& view);

    void SetGeometryShader(Shader* shader) { m_GeometryShader = shader; }
    void SetLightingShader(Shader* shader) { m_LightingShader = shader; }
    void SetLightBoxShader(Shader* shader) { m_LightBoxShader = shader; }

    LightManager& GetLightManager() { return m_LightManager; }
    const LightManager& GetLightManager() const { return m_LightManager; }

    std::shared_ptr<GBuffer> GetGBuffer() const { return m_GBuffer; }

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

private:
    std::shared_ptr<GBuffer> m_GBuffer;
    LightManager m_LightManager;

    Shader* m_GeometryShader;
    Shader* m_LightingShader;
    Shader* m_LightBoxShader;

    unsigned int m_Width;
    unsigned int m_Height;

    unsigned int m_CubeVAO;
    unsigned int m_CubeVBO;
    bool m_CubeInitialized;

    void InitCube();
    void RenderCube();
    void RenderQuad();
};

#endif
