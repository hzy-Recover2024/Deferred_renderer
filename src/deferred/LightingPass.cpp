/**
 * @file LightingPass.cpp
 * @brief 光照通道类实现 / Lighting pass class implementation
 */

#include "deferred/LightingPass.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief 构造函数实现 / Constructor implementation
 */
LightingPass::LightingPass(const std::string& name, RefPtr<GBuffer> gbuffer)
    : RenderPass(name), m_GBuffer(gbuffer), m_Shader(nullptr), m_LightManager(nullptr),
      m_CameraPosition(0.0f), m_RenderTargetFBO(0), m_Width(800), m_Height(600)
{
}

/**
 * @brief 设置通道 / Setup pass
 */
void LightingPass::Setup()
{
}

/**
 * @brief 执行渲染 / Execute rendering
 * 
 * 执行光照通道渲染流程：
 * Executes lighting pass rendering flow:
 * 1. 复制深度缓冲到渲染目标 / Copy depth buffer to render target
 * 2. 绑定 G-Buffer 纹理 / Bind G-Buffer textures
 * 3. 设置着色器参数 / Set shader parameters
 * 4. 上传光源数据 / Upload light data
 * 5. 渲染全屏四边形 / Render fullscreen quad
 */
void LightingPass::Execute()
{
    if (!m_GBuffer || !m_Shader)
        return;

    /// 复制深度缓冲到渲染目标 / Copy depth buffer to render target
    /// 这允许后续的前向渲染正确进行深度测试
    /// This allows subsequent forward rendering to perform correct depth testing
    unsigned int gbufferFBO = m_GBuffer->GetFramebuffer()->FBO;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gbufferFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RenderTargetFBO);
    glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    /// 绑定渲染目标 / Bind render target
    glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTargetFBO);

    /// 绑定 G-Buffer 纹理用于读取 / Bind G-Buffer textures for reading
    m_GBuffer->BindForReading();

    /// 使用光照着色器 / Use lighting shader
    m_Shader->use();

    /// 设置 G-Buffer 纹理单元 / Set G-Buffer texture units
    m_Shader->setInt("gPosition", 0);
    m_Shader->setInt("gNormal", 1);
    m_Shader->setInt("gAlbedoSpec", 2);

    /// 设置相机位置 / Set camera position
    m_Shader->setVec3("viewPos", m_CameraPosition);

    /// 上传光源数据 / Upload light data
    if (m_LightManager)
    {
        const auto& lights = m_LightManager->GetPointLights();
        const int NR_LIGHTS = 32;

        m_Shader->setInt("numLights", static_cast<int>(lights.size()));

        for (size_t i = 0; i < lights.size() && i < NR_LIGHTS; ++i)
        {
            std::string prefix = "lights[" + std::to_string(i) + "].";
            m_Shader->setVec3(prefix + "Position", lights[i].Position);
            m_Shader->setVec3(prefix + "Color", lights[i].Color);

            m_Shader->setFloat(prefix + "Linear", lights[i].Linear);
            m_Shader->setFloat(prefix + "Quadratic", lights[i].Quadratic);
        }
    }

    /// 渲染全屏四边形 / Render fullscreen quad
    ScreenQuad::GetInstance().Render();
}
