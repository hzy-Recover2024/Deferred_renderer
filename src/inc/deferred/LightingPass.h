/**
 * @file LightingPass.h
 * @brief 光照通道类定义 / Lighting pass class definition
 * 
 * 延迟渲染的光照通道，负责从 G-Buffer 读取几何信息并计算光照。
 * Lighting pass for deferred rendering, responsible for reading geometry 
 * from G-Buffer and computing lighting.
 */

#ifndef LIGHTING_PASS_H
#define LIGHTING_PASS_H

#include "rendergraph/RenderPass.h"
#include "rendergraph/ShaderManager.h"
#include "rendergraph/ScreenQuad.h"
#include "GBuffer.h"
#include "Light.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>

/**
 * @brief 光照通道类 / Lighting pass class
 * 
 * 继承自 RenderPass，负责延迟渲染的第二阶段：
 * Inherits from RenderPass, responsible for the second stage of deferred rendering:
 * 
 * 1. 复制深度缓冲到渲染目标 / Copy depth buffer to render target
 * 2. 绑定 G-Buffer 纹理 / Bind G-Buffer textures
 * 3. 设置光照参数 / Set lighting parameters
 * 4. 渲染全屏四边形计算光照 / Render fullscreen quad to compute lighting
 * 
 * 渲染目标 / Render target:
 * - 默认渲染到屏幕 (FBO = 0)
 * - 可通过 SetRenderTarget() 设置自定义 FBO
 */
class LightingPass : public RenderPass
{
public:
    /**
     * @brief 构造函数 / Constructor
     * @param name 通道名称 / Pass name
     * @param gbuffer G-Buffer 引用 / G-Buffer reference
     */
    LightingPass(const std::string& name, RefPtr<GBuffer> gbuffer);

    /**
     * @brief 析构函数 / Destructor
     */
    ~LightingPass() = default;

    /**
     * @brief 设置通道 / Setup pass
     * 
     * 添加 G-Buffer 作为输入资源。
     * Adds G-Buffer as input resource.
     */
    void Setup() override;

    /**
     * @brief 执行渲染 / Execute rendering
     * 
     * 执行光照通道渲染流程。
     * Executes lighting pass rendering flow.
     */
    void Execute() override;

    /**
     * @brief 设置光照着色器 / Set lighting shader
     * @param shader 着色器智能指针 / Shader smart pointer
     */
    void SetShader(RefPtr<Shader> shader) { m_Shader = shader; }

    /**
     * @brief 设置光源管理器 / Set light manager
     * @param manager 光源管理器指针（观察者指针，不拥有所有权）
     *                 / Light manager pointer (observer pointer, no ownership)
     */
    void SetLightManager(LightManager* manager) { m_LightManager = manager; }

    /**
     * @brief 设置相机位置 / Set camera position
     * @param pos 相机位置 / Camera position
     */
    void SetCameraPosition(const glm::vec3& pos) { m_CameraPosition = pos; }

    /**
     * @brief 设置渲染目标 / Set render target
     * 
     * 设置光照计算的输出目标。
     * Sets the output target for lighting calculation.
     * 
     * @param fbo 帧缓冲对象 ID，0 表示默认帧缓冲（屏幕）
     *            / FBO ID, 0 for default framebuffer (screen)
     */
    void SetRenderTarget(unsigned int fbo) { m_RenderTargetFBO = fbo; }

    /**
     * @brief 设置屏幕尺寸 / Set screen dimensions
     * 
     * 用于深度缓冲复制。
     * Used for depth buffer copying.
     * 
     * @param width 宽度 / Width
     * @param height 高度 / Height
     */
    void SetScreenSize(unsigned int width, unsigned int height)
    {
        m_Width = width;
        m_Height = height;
    }

    /**
     * @brief 获取着色器 / Get shader
     * @return RefPtr<Shader> 着色器智能指针 / Shader smart pointer
     */
    RefPtr<Shader> GetShader() const { return m_Shader; }

    /**
     * @brief 获取渲染目标 / Get render target
     * @return unsigned int FBO ID
     */
    unsigned int GetRenderTarget() const { return m_RenderTargetFBO; }

private:
    RefPtr<GBuffer> m_GBuffer;          ///< G-Buffer / G-Buffer
    RefPtr<Shader> m_Shader;            ///< 光照着色器 / Lighting shader
    LightManager* m_LightManager;       ///< 光源管理器（观察者指针）/ Light manager (observer pointer)
    glm::vec3 m_CameraPosition;         ///< 相机位置 / Camera position
    unsigned int m_RenderTargetFBO;     ///< 渲染目标 FBO / Render target FBO
    unsigned int m_Width;               ///< 屏幕宽度 / Screen width
    unsigned int m_Height;              ///< 屏幕高度 / Screen height
};

#endif
