/**
 * @file GeometryPass.cpp
 * @brief 几何通道类实现 / Geometry pass class implementation
 */

#include "deferred/GeometryPass.h"

/**
 * @brief 构造函数实现 / Constructor implementation
 */
GeometryPass::GeometryPass(const std::string& name, RefPtr<GBuffer> gbuffer)
    : RenderPass(name), m_GBuffer(gbuffer), m_Shader(nullptr), 
      m_Projection(1.0f), m_View(1.0f), m_RenderCallback(nullptr)
{
}

/**
 * @brief 设置通道 / Setup pass
 */
void GeometryPass::Setup()
{
    if (m_GBuffer)
    {
        AddOutput(m_GBuffer->GetFramebuffer());
    }
}

/**
 * @brief 执行渲染 / Execute rendering
 * 
 * 执行几何通道渲染流程：
 * Executes geometry pass rendering flow:
 * 1. 绑定 G-Buffer / Bind G-Buffer
 * 2. 清除缓冲 / Clear buffers
 * 3. 设置着色器参数 / Set shader parameters
 * 4. 调用渲染回调 / Call render callback
 */
void GeometryPass::Execute()
{
    if (!m_GBuffer || !m_Shader)
        return;

    /// 绑定 G-Buffer 进行写入 / Bind G-Buffer for writing
    m_GBuffer->BindForWriting();

    /// 清除颜色和深度缓冲 / Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// 设置着色器参数 / Set shader parameters
    m_Shader->use();
    m_Shader->setMat4("projection", m_Projection);
    m_Shader->setMat4("view", m_View);

    /// 调用渲染回调绘制场景 / Call render callback to draw scene
    if (m_RenderCallback)
    {
        m_RenderCallback(m_Shader.get());
    }

    /// 解绑 G-Buffer / Unbind G-Buffer
    m_GBuffer->Unbind();
}
