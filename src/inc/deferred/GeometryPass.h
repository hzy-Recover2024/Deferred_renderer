/**
 * @file GeometryPass.h
 * @brief 几何通道
 * 
 * 延迟渲染的几何通道，负责将场景几何信息写入 G-Buffer。
 */

#ifndef GEOMETRY_PASS_H
#define GEOMETRY_PASS_H

#include "RenderPass.h"
#include "ShaderManager.h"
#include "GBuffer.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>
#include <functional>

/**
 * @brief 几何通道类
 * 
 * 负责延迟渲染的第一阶段：
 * 1. 绑定 G-Buffer 帧缓冲
 * 2. 清除缓冲
 * 3. 设置着色器参数
 * 4. 调用渲染回调绘制场景
 */
class GeometryPass : public RenderPass
{
public:
    using RenderCallback = std::function<void(Shader* shader)>;

    GeometryPass(const std::string& name, RefPtr<GBuffer> gbuffer);
    ~GeometryPass() = default;

    void Setup() override;
    void Execute() override;

    void SetProjection(const glm::mat4& projection) { m_Projection = projection; }
    void SetView(const glm::mat4& view) { m_View = view; }
    void SetShader(RefPtr<Shader> shader) { m_Shader = shader; }
    void SetRenderCallback(RenderCallback callback) { m_RenderCallback = callback; }

    RefPtr<GBuffer> GetGBuffer() const { return m_GBuffer; }
    RefPtr<Shader> GetShader() const { return m_Shader; }

private:
    RefPtr<GBuffer> m_GBuffer;
    RefPtr<Shader> m_Shader;
    glm::mat4 m_Projection;
    glm::mat4 m_View;
    RenderCallback m_RenderCallback;
};

#endif
