/**
 * @file GeometryPass.h
 * @brief 几何通道类定义 / Geometry pass class definition
 * 
 * 延迟渲染的几何通道，负责将场景几何信息写入 G-Buffer。
 * Geometry pass for deferred rendering, responsible for writing scene geometry to G-Buffer.
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
 * @brief 几何通道类 / Geometry pass class
 * 
 * 继承自 RenderPass，负责延迟渲染的第一阶段：
 * Inherits from RenderPass, responsible for the first stage of deferred rendering:
 * 
 * 1. 绑定 G-Buffer 帧缓冲 / Bind G-Buffer framebuffer
 * 2. 清除缓冲 / Clear buffers
 * 3. 设置着色器参数 / Set shader parameters
 * 4. 调用渲染回调绘制场景 / Call render callback to draw scene
 * 
 * 使用回调模式让外部提供场景渲染逻辑。
 * Uses callback pattern to let external code provide scene rendering logic.
 */
class GeometryPass : public RenderPass
{
public:
    /**
     * @brief 渲染回调类型 / Render callback type
     * 
     * 参数：Shader* shader - 几何着色器指针
     * Parameter: Shader* shader - Geometry shader pointer
     */
    using RenderCallback = std::function<void(Shader* shader)>;

    /**
     * @brief 构造函数 / Constructor
     * @param name 通道名称 / Pass name
     * @param gbuffer G-Buffer 引用 / G-Buffer reference
     */
    GeometryPass(const std::string& name, RefPtr<GBuffer> gbuffer);

    /**
     * @brief 析构函数 / Destructor
     */
    ~GeometryPass() = default;

    /**
     * @brief 设置通道 / Setup pass
     * 
     * 添加 G-Buffer 作为输出资源。
     * Adds G-Buffer as output resource.
     */
    void Setup() override;

    /**
     * @brief 执行渲染 / Execute rendering
     * 
     * 执行几何通道渲染流程。
     * Executes geometry pass rendering flow.
     */
    void Execute() override;

    /**
     * @brief 设置投影矩阵 / Set projection matrix
     * @param projection 投影矩阵 / Projection matrix
     */
    void SetProjection(const glm::mat4& projection) { m_Projection = projection; }

    /**
     * @brief 设置视图矩阵 / Set view matrix
     * @param view 视图矩阵 / View matrix
     */
    void SetView(const glm::mat4& view) { m_View = view; }

    /**
     * @brief 设置几何着色器 / Set geometry shader
     * @param shader 着色器智能指针 / Shader smart pointer
     */
    void SetShader(RefPtr<Shader> shader) { m_Shader = shader; }

    /**
     * @brief 设置渲染回调 / Set render callback
     * @param callback 渲染回调函数 / Render callback function
     */
    void SetRenderCallback(RenderCallback callback) { m_RenderCallback = callback; }

    /**
     * @brief 获取 G-Buffer / Get G-Buffer
     * @return G-Buffer 引用 / G-Buffer reference
     */
    RefPtr<GBuffer> GetGBuffer() const { return m_GBuffer; }

    /**
     * @brief 获取着色器 / Get shader
     * @return RefPtr<Shader> 着色器智能指针 / Shader smart pointer
     */
    RefPtr<Shader> GetShader() const { return m_Shader; }

private:
    RefPtr<GBuffer> m_GBuffer;       ///< G-Buffer / G-Buffer
    RefPtr<Shader> m_Shader;         ///< 几何着色器 / Geometry shader
    glm::mat4 m_Projection;          ///< 投影矩阵 / Projection matrix
    glm::mat4 m_View;                ///< 视图矩阵 / View matrix
    RenderCallback m_RenderCallback; ///< 渲染回调 / Render callback
};

#endif
