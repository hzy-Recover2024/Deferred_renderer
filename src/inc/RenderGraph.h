/**
 * @file RenderGraph.h
 * @brief 渲染图管理类
 * 
 * 渲染图是整个渲染管线的核心管理类，负责管理所有渲染通道和资源，
 * 并根据依赖关系自动排序执行。
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
 * @brief 渲染图类
 * 
 * 管理整个渲染管线，提供：
 * 1. 资源创建和管理
 * 2. 渲染通道创建和管理
 * 3. 依赖关系解析和拓扑排序
 * 4. 环检测
 * 5. 自动资源验证
 */
class RenderGraph
{
public:
    RenderGraph();
    ~RenderGraph() = default;

    template<typename T, typename... Args>
    RefPtr<T> CreateResource(Args&&... args)
    {
        auto resource = MakeRefPtr<T>(std::forward<Args>(args)...);
        m_Resources.push_back(resource);
        m_ResourceMap[resource->Name] = resource;
        m_NeedsRecompile = true;
        return resource;
    }

    template<typename T, typename... Args>
    RefPtr<T> CreatePass(Args&&... args)
    {
        auto pass = MakeRefPtr<T>(std::forward<Args>(args)...);
        m_Passes.push_back(pass);
        m_PassMap[pass->Name] = pass;
        m_NeedsRecompile = true;
        return pass;
    }

    void SetDirty() { m_NeedsRecompile = true; }
    bool NeedsRecompile() const { return m_NeedsRecompile; }

    /**
     * @brief 编译渲染图
     * 
     * 执行以下步骤（仅当需要时）：
     * 1. 检测依赖环
     * 2. 拓扑排序通道
     * 3. 验证资源有效性
     * 4. 准备所有通道
     */
    void Compile();

    void ForceRecompile();

    /**
     * @brief 执行渲染图
     * 
     * 按拓扑排序顺序执行所有渲染通道。
     * 如果渲染图未编译，会自动调用 Compile()。
     */
    void Execute();

    void Clear();

    RefPtr<RenderPass> GetPass(const std::string& name) const;
    RefPtr<RenderResource> GetResource(const std::string& name) const;

    void SetBackbufferWidth(unsigned int width) { m_BackbufferWidth = width; }
    void SetBackbufferHeight(unsigned int height) { m_BackbufferHeight = height; }
    unsigned int GetBackbufferWidth() const { return m_BackbufferWidth; }
    unsigned int GetBackbufferHeight() const { return m_BackbufferHeight; }

private:
    void TopologicalSort();
    bool DetectCycle();
    void ValidateResources();

    std::vector<RefPtr<RenderPass>> m_Passes;
    std::vector<RefPtr<RenderResource>> m_Resources;
    std::unordered_map<std::string, RefPtr<RenderPass>> m_PassMap;
    std::unordered_map<std::string, RefPtr<RenderResource>> m_ResourceMap;

    std::vector<RefPtr<RenderPass>> m_SortedPasses;

    unsigned int m_BackbufferWidth = 800;
    unsigned int m_BackbufferHeight = 600;

    bool m_NeedsRecompile = true;
    bool m_IsCompiled = false;
};

#endif
