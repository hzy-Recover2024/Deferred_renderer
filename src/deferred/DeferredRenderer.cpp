/**
 * @file DeferredRenderer.cpp
 * @brief 延迟渲染器类实现 / Deferred renderer class implementation
 */

#include "deferred/DeferredRenderer.h"
#include <iostream>

/**
 * @brief 构造函数实现 / Constructor implementation
 */
DeferredRenderer::DeferredRenderer()
    : m_GeometryPass(nullptr), m_LightingPass(nullptr),
      m_GeometryShader(nullptr), m_LightingShader(nullptr),
      m_Projection(1.0f), m_View(1.0f), m_CameraPosition(0.0f),
      m_Width(0), m_Height(0), m_RenderTargetFBO(0), m_Initialized(false)
{
}

// ==================== 初始化（仅一次）====================

/**
 * @brief 初始化渲染器 / Initialize renderer
 * 
 * 仅在启动时调用一次。
 * Called only once at startup.
 */
bool DeferredRenderer::Init(unsigned int width, unsigned int height, const std::string& shaderPath)
{
    if (m_Initialized)
    {
        std::cerr << "DeferredRenderer: Already initialized!" << std::endl;
        return false;
    }

    m_Width = width;
    m_Height = height;

    /// 创建 G-Buffer / Create G-Buffer
    m_GBuffer = std::make_shared<GBuffer>();
    if (!m_GBuffer->Init(width, height))
    {
        std::cerr << "DeferredRenderer: Failed to initialize G-Buffer!" << std::endl;
        return false;
    }

    /// 加载着色器 / Load shaders
    if (!LoadShaders(shaderPath))
    {
        std::cerr << "DeferredRenderer: Failed to load shaders!" << std::endl;
        return false;
    }

    /// 组装渲染图 / Assemble render graph
    AssembleRenderGraph();

    m_Initialized = true;
    return true;
}

/**
 * @brief 加载着色器 / Load shaders
 * 
 * 仅在初始化时调用。
 * Called only during initialization.
 */
bool DeferredRenderer::LoadShaders(const std::string& shaderPath)
{
    auto& shaderManager = ShaderManager::GetInstance();

    /// 加载几何通道着色器 / Load geometry pass shader
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

    /// 加载光照通道着色器 / Load lighting pass shader
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

/**
 * @brief 组装渲染图 / Assemble render graph
 * 
 * 仅在初始化时调用，创建所有 Pass 并设置依赖关系。
 * Called only during initialization, creates all passes and sets dependencies.
 */
void DeferredRenderer::AssembleRenderGraph()
{
    /// 创建几何通道 / Create geometry pass
    auto geometryPass = m_RenderGraph.CreatePass<GeometryPass>("GeometryPass", m_GBuffer);
    geometryPass->SetShader(m_GeometryShader);
    m_GeometryPass = geometryPass.get();

    /// 创建光照通道 / Create lighting pass
    auto lightingPass = m_RenderGraph.CreatePass<LightingPass>("LightingPass", m_GBuffer);
    lightingPass->SetShader(m_LightingShader);
    lightingPass->SetLightManager(&m_LightManager);
    lightingPass->SetScreenSize(m_Width, m_Height);
    m_LightingPass = lightingPass.get();

    /// 设置依赖关系 / Set dependencies
    /// LightingPass 依赖 GeometryPass 的输出
    lightingPass->AddDependency(geometryPass);
}

// ==================== 每帧更新 ====================

/**
 * @brief 设置相机参数 / Set camera parameters
 * 
 * 每帧调用，立即更新所有 Pass 的相机相关参数。
 * Called every frame, immediately updates camera-related parameters for all passes.
 */
void DeferredRenderer::SetCamera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position)
{
    m_Projection = projection;
    m_View = view;
    m_CameraPosition = position;

    /// 立即更新所有 Pass 的参数 / Immediately update parameters for all passes
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

/**
 * @brief 设置渲染目标 / Set render target
 * 
 * 设置最终渲染输出目标。当需要渲染到纹理时调用。
 * Sets the final render output target. Called when rendering to texture is needed.
 * 
 * 渲染目标会传递给 LightingPass。
 * Render target is passed to LightingPass.
 */
void DeferredRenderer::SetRenderTarget(unsigned int fbo)
{
    m_RenderTargetFBO = fbo;

    /// 更新光照通道的渲染目标 / Update render target for lighting pass
    if (m_LightingPass)
    {
        m_LightingPass->SetRenderTarget(fbo);
    }
}

/**
 * @brief 调整渲染器尺寸 / Resize renderer
 * 
 * 当窗口大小改变时调用，调整 G-Buffer 和相关资源。
 * Called when window size changes, resizes G-Buffer and related resources.
 */
void DeferredRenderer::Resize(unsigned int width, unsigned int height)
{
    if (!m_Initialized)
        return;

    /// 尺寸未变化则跳过 / Skip if size hasn't changed
    if (m_Width == width && m_Height == height)
        return;

    m_Width = width;
    m_Height = height;

    /// 调整 G-Buffer 尺寸 / Resize G-Buffer
    if (m_GBuffer)
    {
        m_GBuffer->Resize(width, height);
    }

    /// 更新光照通道屏幕尺寸 / Update lighting pass screen size
    if (m_LightingPass)
    {
        m_LightingPass->SetScreenSize(width, height);
    }

    /// 标记渲染图需要重新编译 / Mark render graph for recompilation
    m_RenderGraph.SetDirty();
}

/**
 * @brief 渲染一帧 / Render a frame
 * 
 * 每帧调用，执行完整的延迟渲染流程。
 * Called every frame, executes complete deferred rendering flow.
 */
void DeferredRenderer::Render()
{
    if (!m_Initialized)
    {
        std::cerr << "DeferredRenderer: Not initialized!" << std::endl;
        return;
    }

    /// 编译渲染图（仅当结构变化时）/ Compile render graph (only when structure changes)
    m_RenderGraph.Compile();

    /// 执行渲染图 / Execute render graph
    m_RenderGraph.Execute();
}

// ==================== 场景数据设置 ====================

/**
 * @brief 设置场景渲染回调 / Set scene render callback
 */
void DeferredRenderer::SetSceneRenderCallback(SceneRenderCallback callback)
{
    if (m_GeometryPass)
    {
        m_GeometryPass->SetRenderCallback(callback);
    }
}
