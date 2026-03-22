/**
 * @file LightingPass.h
 * @brief 光照通道
 * 
 * 延迟渲染的光照通道，负责从 G-Buffer 读取几何信息并计算光照。
 */

#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H

#include "RenderPass.h"
#include "ShaderManager.h"
#include "ScreenQuad.h"
#include "GBuffer.h"
#include "Light.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>

/**
 * @brief 光照通道类
 * 
 * 负责延迟渲染的第二阶段：
 * 1. 复制深度缓冲到渲染目标
 * 2. 绑定 G-Buffer 纹理
 * 3. 设置光照参数
 * 4. 渲染全屏四边形计算光照
 */
class LightingPass : public RenderPass
{
public:
    LightingPass(const std::string& name, RefPtr<GBuffer> gbuffer);
    ~LightingPass() = default;

    void Setup() override;
    void Execute() override;

    void SetShader(RefPtr<Shader> shader) { m_Shader = shader; }
    void SetLightManager(LightManager* manager) { m_LightManager = manager; }
    void SetCameraPosition(const glm::vec3& pos) { m_CameraPosition = pos; }

    /**
     * @brief 设置渲染目标
     * 
     * @param fbo 帧缓冲对象 ID，0 表示默认帧缓冲（屏幕）
     */
    void SetRenderTarget(unsigned int fbo) { m_RenderTargetFBO = fbo; }

    /**
     * @brief 设置屏幕尺寸
     * 
     * 用于深度缓冲复制。
     */
    void SetScreenSize(unsigned int width, unsigned int height)
    {
        m_Width = width;
        m_Height = height;
    }

    RefPtr<Shader> GetShader() const { return m_Shader; }
    unsigned int GetRenderTarget() const { return m_RenderTargetFBO; }

private:
    RefPtr<GBuffer> m_GBuffer;
    RefPtr<Shader> m_Shader;
    LightManager* m_LightManager;
    glm::vec3 m_CameraPosition;
    unsigned int m_RenderTargetFBO;
    unsigned int m_Width;
    unsigned int m_Height;
};

#endif
