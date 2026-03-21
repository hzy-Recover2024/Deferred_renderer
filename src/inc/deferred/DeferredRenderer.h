/**
 * @file DeferredRenderer.h
 * @brief 延迟渲染器类定义 / Deferred renderer class definition
 * 
 * 基于 RenderGraph 的延迟渲染器，组装和管理渲染通道。
 * RenderGraph-based deferred renderer, assembles and manages render passes.
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

/**
 * @brief 延迟渲染器类 / Deferred renderer class
 * 
 * 使用 RenderGraph 架构的延迟渲染器：
 * Deferred renderer using RenderGraph architecture:
 * 
 * 职责 / Responsibilities:
 * 1. 创建延迟渲染管线所需的 Shader / Create shaders for deferred rendering pipeline
 * 2. 构建 RenderGraph / Build RenderGraph
 * 3. 管理场景数据 / Manage scene data
 * 
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │                         DeferredRenderer                                 │
 * │                                                                          │
 * │  【初始化时 - 仅一次】                                                     │
 * │  ├─ 创建 G-Buffer                                                        │
 * │  ├─ 加载 Shader（通过 ShaderManager）                                     │
 * │  └─ 组装 RenderGraph                                                     │
 * │                                                                          │
 * │  【每帧必须】                                                              │
 * │  ├─ 1. 更新相机参数 SetCamera()                                           │
 * │  ├─ 2. 更新渲染目标 SetRenderTarget() - 可选                              │
 * │  ├─ 3. 更新屏幕尺寸 Resize() - 可选                                       │
 * │  └─ 4. 执行渲染 Render()                                                  │
 * │                                                                          │
 * │  ┌───────────────────────────────────────────────────────────────┐     │
 * │  │                      RenderGraph                               │     │
 * │  │                                                                │     │
 * │  │  Passes:                                                       │     │
 * │  │  ┌──────────────┐    ┌──────────────┐                         │     │
 * │  │  │ GeometryPass │───►│ LightingPass │                         │     │
 * │  │  │  写入 GBuffer │    │  读取 GBuffer │                         │     │
 * │  │  └──────────────┘    └──────────────┘                         │     │
 * │  │                                                                │     │
 * │  └───────────────────────────────────────────────────────────────┘     │
 * │                                                                          │
 * └─────────────────────────────────────────────────────────────────────────┘
 */
class DeferredRenderer
{
public:
    /**
     * @brief 场景渲染回调类型 / Scene render callback type
     * 
     * 参数：Shader* shader - 几何着色器指针
     * Parameter: Shader* shader - Geometry shader pointer
     */
    using SceneRenderCallback = std::function<void(Shader* shader)>;

    /**
     * @brief 构造函数 / Constructor
     */
    DeferredRenderer();

    /**
     * @brief 析构函数 / Destructor
     */
    ~DeferredRenderer() = default;

    // ==================== 初始化（仅一次）====================
    
    /**
     * @brief 初始化渲染器 / Initialize renderer
     * 
     * 创建 G-Buffer、加载 Shader、组装 RenderGraph。
     * Creates G-Buffer, loads shaders, assembles RenderGraph.
     * 
     * @param width 屏幕宽度 / Screen width
     * @param height 屏幕高度 / Screen height
     * @param shaderPath 着色器目录路径 / Shader directory path
     * @return true 初始化成功 / Initialization successful
     * @return false 初始化失败 / Initialization failed
     */
    bool Init(unsigned int width, unsigned int height, const std::string& shaderPath = "shader");

    // ==================== 每帧更新 ====================

    /**
     * @brief 设置相机参数 / Set camera parameters
     * 
     * 每帧调用，更新所有 Pass 的相机相关参数。
     * Called every frame, updates camera-related parameters for all passes.
     * 
     * @param projection 投影矩阵 / Projection matrix
     * @param view 视图矩阵 / View matrix
     * @param position 相机位置 / Camera position
     */
    void SetCamera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position);

    /**
     * @brief 设置渲染目标 / Set render target
     * 
     * 设置最终渲染输出目标。默认为 0（默认帧缓冲，即屏幕）。
     * Sets the final render output target. Default is 0 (default framebuffer, i.e., screen).
     * 
     * @param fbo 帧缓冲对象 ID，0 表示默认帧缓冲 / FBO ID, 0 for default framebuffer
     */
    void SetRenderTarget(unsigned int fbo);

    /**
     * @brief 调整渲染器尺寸 / Resize renderer
     * 
     * 当窗口大小改变时调用，调整 G-Buffer 和相关资源。
     * Called when window size changes, resizes G-Buffer and related resources.
     * 
     * @param width 新宽度 / New width
     * @param height 新高度 / New height
     */
    void Resize(unsigned int width, unsigned int height);

    /**
     * @brief 渲染一帧 / Render a frame
     * 
     * 执行完整的延迟渲染流程。每帧调用。
     * Executes complete deferred rendering flow. Called every frame.
     * 
     * 执行顺序 / Execution order:
     * 1. GeometryPass - 写入 G-Buffer
     * 2. LightingPass - 计算光照，复制深度缓冲
     */
    void Render();

    // ==================== 场景数据设置 ====================

    /**
     * @brief 设置场景渲染回调 / Set scene render callback
     * @param callback 渲染回调函数 / Render callback function
     */
    void SetSceneRenderCallback(SceneRenderCallback callback);

    // ==================== 访问器 ====================

    /**
     * @brief 获取光源管理器 / Get light manager
     * @return LightManager& 光源管理器引用 / Light manager reference
     */
    LightManager& GetLightManager() { return m_LightManager; }
    const LightManager& GetLightManager() const { return m_LightManager; }

    /**
     * @brief 获取 G-Buffer / Get G-Buffer
     * @return RefPtr<GBuffer> G-Buffer 引用 / G-Buffer reference
     */
    RefPtr<GBuffer> GetGBuffer() const { return m_GBuffer; }

    /**
     * @brief 获取 RenderGraph / Get RenderGraph
     * @return RenderGraph& RenderGraph 引用 / RenderGraph reference
     */
    RenderGraph& GetRenderGraph() { return m_RenderGraph; }
    const RenderGraph& GetRenderGraph() const { return m_RenderGraph; }

    /**
     * @brief 获取几何着色器 / Get geometry shader
     * @return RefPtr<Shader> 着色器智能指针 / Shader smart pointer
     */
    RefPtr<Shader> GetGeometryShader() const { return m_GeometryShader; }

    /**
     * @brief 获取光照着色器 / Get lighting shader
     * @return RefPtr<Shader> 着色器智能指针 / Shader smart pointer
     */
    RefPtr<Shader> GetLightingShader() const { return m_LightingShader; }

    /**
     * @brief 获取屏幕尺寸 / Get screen dimensions
     */
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

    /**
     * @brief 获取当前渲染目标 / Get current render target
     * @return unsigned int FBO ID
     */
    unsigned int GetRenderTarget() const { return m_RenderTargetFBO; }

    /**
     * @brief 是否已初始化 / Whether initialized
     */
    bool IsInitialized() const { return m_Initialized; }

private:
    /// 渲染图 / Render graph
    RenderGraph m_RenderGraph;

    /// G-Buffer
    RefPtr<GBuffer> m_GBuffer;

    /// 渲染通道（观察者指针，由 RenderGraph 管理）
    /// Render passes (observer pointers, managed by RenderGraph)
    GeometryPass* m_GeometryPass;
    LightingPass* m_LightingPass;

    /// 光源管理器 / Light manager
    LightManager m_LightManager;

    /// 着色器（由 ShaderManager 管理）
    /// Shaders (managed by ShaderManager)
    RefPtr<Shader> m_GeometryShader;
    RefPtr<Shader> m_LightingShader;

    /// 相机参数 / Camera parameters
    glm::mat4 m_Projection;
    glm::mat4 m_View;
    glm::vec3 m_CameraPosition;

    /// 屏幕尺寸 / Screen dimensions
    unsigned int m_Width;
    unsigned int m_Height;

    /// 渲染目标 FBO（0 = 默认帧缓冲）/ Render target FBO (0 = default framebuffer)
    unsigned int m_RenderTargetFBO;

    /// 是否已初始化 / Whether initialized
    bool m_Initialized;

    /**
     * @brief 加载着色器 / Load shaders
     * @param shaderPath 着色器目录路径 / Shader directory path
     * @return true 加载成功 / Loading successful
     * @return false 加载失败 / Loading failed
     */
    bool LoadShaders(const std::string& shaderPath);

    /**
     * @brief 组装渲染图 / Assemble render graph
     */
    void AssembleRenderGraph();
};

#endif
