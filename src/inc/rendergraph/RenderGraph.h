/**
 * @file RenderGraph.h
 * @brief 渲染图管理类定义 / Render graph management class definition
 * 
 * 渲染图是整个渲染管线的核心管理类，负责管理所有渲染通道和资源，
 * 并根据依赖关系自动排序执行。
 * Render graph is the core management class of the entire rendering pipeline,
 * responsible for managing all render passes and resources,
 * and automatically sorting execution based on dependencies.
 */

#ifndef RENDER_GRAPH_H
#define RENDER_GRAPH_H

#include "RenderPass.h"
#include "RenderResource.h"
#include "Texture.h"
#include "Framebuffer.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

/**
 * @brief 渲染图类 / Render graph class
 * 
 * 管理整个渲染管线的核心类，提供以下功能：
 * Core class managing the entire rendering pipeline, providing:
 * 
 * 1. 资源创建和管理 / Resource creation and management
 * 2. 渲染通道创建和管理 / Render pass creation and management
 * 3. 依赖关系解析和拓扑排序 / Dependency resolution and topological sorting
 * 4. 环检测 / Cycle detection
 * 5. 自动资源验证 / Automatic resource validation
 * 
 * 执行频率 / Execution frequency:
 * - Compile(): 仅在结构变化时调用 / Only called when structure changes
 * - Execute(): 每帧调用 / Called every frame
 * 
 * 使用流程 / Usage flow:
 * @code
 * RenderGraph graph;
 * 
 * // 创建资源 / Create resources
 * auto tex = graph.CreateResource<Texture>("MyTexture", desc);
 * 
 * // 创建通道 / Create passes
 * auto pass = graph.CreatePass<GeometryPass>("Geometry");
 * 
 * // 设置依赖 / Set dependencies
 * pass->AddDependency(otherPass);
 * 
 * // 编译（仅一次）/ Compile (once)
 * graph.Compile();
 * 
 * // 渲染循环 / Render loop
 * while (rendering)
 * {
 *     graph.Execute();  // 每帧执行 / Execute every frame
 * }
 * 
 * // 清理 / Cleanup
 * graph.Clear();
 * @endcode
 */
class RenderGraph
{
public:
    /**
     * @brief 构造函数 / Constructor
     */
    RenderGraph();

    /**
     * @brief 析构函数 / Destructor
     */
    ~RenderGraph() = default;

    /**
     * @brief 创建渲染资源 / Create render resource
     * 
     * 工厂方法，创建并注册渲染资源。
     * Factory method to create and register render resource.
     * 
     * 创建资源后，渲染图会被标记为需要重新编译。
     * After creating a resource, the render graph will be marked for recompilation.
     * 
     * @tparam T 资源类型 / Resource type
     * @tparam Args 构造函数参数类型 / Constructor argument types
     * @param args 构造函数参数 / Constructor arguments
     * @return RefPtr<T> 资源智能指针 / Resource smart pointer
     */
    template<typename T, typename... Args>
    RefPtr<T> CreateResource(Args&&... args)
    {
        auto resource = MakeRefPtr<T>(std::forward<Args>(args)...);
        m_Resources.push_back(resource);
        m_ResourceMap[resource->Name] = resource;
        m_NeedsRecompile = true;  // 标记需要重新编译 / Mark for recompilation
        return resource;
    }

    /**
     * @brief 创建渲染通道 / Create render pass
     * 
     * 工厂方法，创建并注册渲染通道。
     * Factory method to create and register render pass.
     * 
     * 创建通道后，渲染图会被标记为需要重新编译。
     * After creating a pass, the render graph will be marked for recompilation.
     * 
     * @tparam T 通道类型 / Pass type
     * @tparam Args 构造函数参数类型 / Constructor argument types
     * @param args 构造函数参数 / Constructor arguments
     * @return RefPtr<T> 通道智能指针 / Pass smart pointer
     */
    template<typename T, typename... Args>
    RefPtr<T> CreatePass(Args&&... args)
    {
        auto pass = MakeRefPtr<T>(std::forward<Args>(args)...);
        m_Passes.push_back(pass);
        m_PassMap[pass->Name] = pass;
        m_NeedsRecompile = true;  // 标记需要重新编译 / Mark for recompilation
        return pass;
    }

    /**
     * @brief 标记需要重新编译 / Mark for recompilation
     * 
     * 当渲染图结构发生变化时（如添加/删除依赖），调用此方法。
     * Call this method when render graph structure changes (e.g., adding/removing dependencies).
     */
    void SetDirty() { m_NeedsRecompile = true; }

    /**
     * @brief 检查是否需要重新编译 / Check if recompilation is needed
     * @return true 需要重新编译 / Needs recompilation
     * @return false 无需重新编译 / No recompilation needed
     */
    bool NeedsRecompile() const { return m_NeedsRecompile; }

    /**
     * @brief 编译渲染图 / Compile render graph
     * 
     * 执行以下步骤（仅当需要时）：
     * Performs the following steps (only when needed):
     * 1. 检测依赖环 / Detect dependency cycles
     * 2. 拓扑排序通道 / Topologically sort passes
     * 3. 验证资源有效性 / Validate resource validity
     * 4. 准备所有通道 / Prepare all passes
     * 
     * 如果渲染图未变化，此方法会跳过编译。
     * If the render graph hasn't changed, this method will skip compilation.
     */
    void Compile();

    /**
     * @brief 强制重新编译 / Force recompilation
     * 
     * 无论是否需要，都重新编译渲染图。
     * Recompiles the render graph regardless of whether it's needed.
     */
    void ForceRecompile();

    /**
     * @brief 执行渲染图 / Execute render graph
     * 
     * 按拓扑排序顺序执行所有渲染通道。
     * Execute all render passes in topological order.
     * 
     * 如果渲染图未编译，会自动调用 Compile()。
     * If the render graph hasn't been compiled, Compile() will be called automatically.
     */
    void Execute();

    /**
     * @brief 清理渲染图 / Clear render graph
     * 
     * 清理所有通道和资源。
     * Clears all passes and resources.
     */
    void Clear();

    /**
     * @brief 获取渲染通道 / Get render pass
     * @param name 通道名称 / Pass name
     * @return RefPtr<RenderPass> 通道智能指针，未找到返回 nullptr / Pass smart pointer, nullptr if not found
     */
    RefPtr<RenderPass> GetPass(const std::string& name) const;

    /**
     * @brief 获取渲染资源 / Get render resource
     * @param name 资源名称 / Resource name
     * @return RefPtr<RenderResource> 资源智能指针，未找到返回 nullptr / Resource smart pointer, nullptr if not found
     */
    RefPtr<RenderResource> GetResource(const std::string& name) const;

    /**
     * @brief 设置后缓冲宽度 / Set backbuffer width
     * @param width 宽度（像素）/ Width in pixels
     */
    void SetBackbufferWidth(unsigned int width) { m_BackbufferWidth = width; }

    /**
     * @brief 设置后缓冲高度 / Set backbuffer height
     * @param height 高度（像素）/ Height in pixels
     */
    void SetBackbufferHeight(unsigned int height) { m_BackbufferHeight = height; }

    /**
     * @brief 获取后缓冲宽度 / Get backbuffer width
     * @return 宽度（像素）/ Width in pixels
     */
    unsigned int GetBackbufferWidth() const { return m_BackbufferWidth; }

    /**
     * @brief 获取后缓冲高度 / Get backbuffer height
     * @return 高度（像素）/ Height in pixels
     */
    unsigned int GetBackbufferHeight() const { return m_BackbufferHeight; }

private:
    /**
     * @brief 拓扑排序 / Topological sort
     * 
     * 根据依赖关系对通道进行拓扑排序，结果存储在 m_SortedPasses 中。
     * Topologically sort passes based on dependencies, result stored in m_SortedPasses.
     */
    void TopologicalSort();

    /**
     * @brief 检测依赖环 / Detect dependency cycle
     * @return true 存在环 / Cycle exists
     * @return false 无环 / No cycle
     */
    bool DetectCycle();

    /**
     * @brief 验证资源 / Validate resources
     * 
     * 检查所有资源是否有效，无效则创建。
     * Check if all resources are valid, create if invalid.
     */
    void ValidateResources();

    std::vector<RefPtr<RenderPass>> m_Passes;       ///< 所有渲染通道 / All render passes
    std::vector<RefPtr<RenderResource>> m_Resources; ///< 所有渲染资源 / All render resources
    std::unordered_map<std::string, RefPtr<RenderPass>> m_PassMap;       ///< 通道名称映射 / Pass name map
    std::unordered_map<std::string, RefPtr<RenderResource>> m_ResourceMap; ///< 资源名称映射 / Resource name map

    std::vector<RefPtr<RenderPass>> m_SortedPasses; ///< 排序后的通道 / Sorted passes

    unsigned int m_BackbufferWidth = 800;   ///< 后缓冲宽度 / Backbuffer width
    unsigned int m_BackbufferHeight = 600;  ///< 后缓冲高度 / Backbuffer height

    bool m_NeedsRecompile = true;  ///< 是否需要重新编译 / Whether recompilation is needed
    bool m_IsCompiled = false;     ///< 是否已编译 / Whether compiled
};

#endif
