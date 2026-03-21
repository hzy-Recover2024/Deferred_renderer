/**
 * @file RenderGraph.cpp
 * @brief 渲染图管理类实现 / Render graph management class implementation
 */

#include "RenderGraph.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <queue>

/**
 * @brief 构造函数实现 / Constructor implementation
 */
RenderGraph::RenderGraph()
{
}

/**
 * @brief 编译渲染图 / Compile render graph
 * 
 * 仅在需要时执行编译，避免重复编译。
 * Only compiles when needed, avoiding redundant compilation.
 */
void RenderGraph::Compile()
{
    /// 如果不需要重新编译且已编译过，直接返回 / Skip if no recompilation needed and already compiled
    if (!m_NeedsRecompile && m_IsCompiled)
    {
        return;
    }

    if (DetectCycle())
    {
        std::cerr << "RenderGraph: Cycle detected in pass dependencies!" << std::endl;
        return;
    }

    TopologicalSort();
    ValidateResources();

    for (auto& pass : m_SortedPasses)
    {
        pass->Prepare();
    }

    m_NeedsRecompile = false;
    m_IsCompiled = true;
}

/**
 * @brief 强制重新编译 / Force recompilation
 */
void RenderGraph::ForceRecompile()
{
    m_NeedsRecompile = true;
    m_IsCompiled = false;
    Compile();
}

/**
 * @brief 执行渲染图 / Execute render graph
 * 
 * 按拓扑排序顺序执行所有渲染通道。
 * Execute all render passes in topological order.
 * 
 * 如果未编译，自动调用 Compile()。
 * Automatically calls Compile() if not compiled.
 */
void RenderGraph::Execute()
{
    /// 确保渲染图已编译 / Ensure render graph is compiled
    if (!m_IsCompiled)
    {
        Compile();
    }

    for (auto& pass : m_SortedPasses)
    {
        pass->Run();
    }
}

/**
 * @brief 清理渲染图 / Clear render graph
 * 
 * 清理所有通道和资源，重置编译状态。
 * Clears all passes and resources, resets compilation state.
 */
void RenderGraph::Clear()
{
    for (auto& pass : m_Passes)
    {
        pass->Finish();
    }

    m_Passes.clear();
    m_Resources.clear();
    m_PassMap.clear();
    m_ResourceMap.clear();
    m_SortedPasses.clear();

    /// 重置编译状态 / Reset compilation state
    m_NeedsRecompile = true;
    m_IsCompiled = false;
}

/**
 * @brief 获取渲染通道 / Get render pass
 */
RefPtr<RenderPass> RenderGraph::GetPass(const std::string& name) const
{
    auto it = m_PassMap.find(name);
    if (it != m_PassMap.end())
        return it->second;
    return nullptr;
}

/**
 * @brief 获取渲染资源 / Get render resource
 */
RefPtr<RenderResource> RenderGraph::GetResource(const std::string& name) const
{
    auto it = m_ResourceMap.find(name);
    if (it != m_ResourceMap.end())
        return it->second;
    return nullptr;
}

/**
 * @brief 拓扑排序 / Topological sort
 * 
 * 使用 Kahn 算法根据依赖关系对通道进行拓扑排序。
 * Uses Kahn's algorithm to topologically sort passes based on dependencies.
 * 
 * 算法步骤 / Algorithm steps:
 * 1. 计算每个通道的入度 / Calculate in-degree for each pass
 * 2. 将入度为 0 的通道加入队列 / Add passes with 0 in-degree to queue
 * 3. 依次取出通道，减少其邻居的入度 / Remove passes, decrease neighbors' in-degree
 * 4. 重复直到队列为空 / Repeat until queue is empty
 */
void RenderGraph::TopologicalSort()
{
    m_SortedPasses.clear();

    /// 入度表：记录每个通道被多少通道依赖 / In-degree table: how many passes depend on each pass
    std::unordered_map<RefPtr<RenderPass>, int> inDegree;
    /// 邻接表：记录每个通道被哪些通道依赖 / Adjacency list: which passes depend on each pass
    std::unordered_map<RefPtr<RenderPass>, std::vector<RefPtr<RenderPass>>> adjacencyList;

    /// 初始化入度为 0 / Initialize in-degree to 0
    for (auto& pass : m_Passes)
    {
        inDegree[pass] = 0;
    }

    /// 构建邻接表和入度表 / Build adjacency list and in-degree table
    for (auto& pass : m_Passes)
    {
        for (auto& dep : pass->GetDependencies())
        {
            adjacencyList[dep].push_back(pass);
            inDegree[pass]++;
        }
    }

    /// 将入度为 0 的通道加入队列 / Add passes with 0 in-degree to queue
    std::queue<RefPtr<RenderPass>> queue;
    for (auto& pass : m_Passes)
    {
        if (inDegree[pass] == 0)
        {
            queue.push(pass);
        }
    }

    /// Kahn 算法主循环 / Kahn's algorithm main loop
    while (!queue.empty())
    {
        RefPtr<RenderPass> current = queue.front();
        queue.pop();
        m_SortedPasses.push_back(current);

        for (auto& neighbor : adjacencyList[current])
        {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0)
            {
                queue.push(neighbor);
            }
        }
    }
}

/**
 * @brief 检测依赖环 / Detect dependency cycle
 * 
 * 使用 DFS 检测依赖图中是否存在环。
 * Uses DFS to detect cycles in the dependency graph.
 * 
 * @return true 存在环 / Cycle exists
 * @return false 无环 / No cycle
 */
bool RenderGraph::DetectCycle()
{
    /// 已访问集合 / Visited set
    std::unordered_set<RefPtr<RenderPass>> visited;
    /// 递归栈，用于检测回边 / Recursion stack for detecting back edges
    std::unordered_set<RefPtr<RenderPass>> recursionStack;

    /// DFS 递归检测环 / DFS recursive cycle detection
    std::function<bool(RefPtr<RenderPass>)> hasCycle = [&](RefPtr<RenderPass> pass) -> bool {
        /// 如果在递归栈中，说明存在环 / If in recursion stack, cycle exists
        if (recursionStack.count(pass))
            return true;

        /// 如果已访问过，跳过 / If already visited, skip
        if (visited.count(pass))
            return false;

        visited.insert(pass);
        recursionStack.insert(pass);

        /// 递归检查所有依赖 / Recursively check all dependencies
        for (auto& dep : pass->GetDependencies())
        {
            if (hasCycle(dep))
                return true;
        }

        /// 回溯，移出递归栈 / Backtrack, remove from recursion stack
        recursionStack.erase(pass);
        return false;
    };

    /// 检查所有通道 / Check all passes
    for (auto& pass : m_Passes)
    {
        if (hasCycle(pass))
            return true;
    }

    return false;
}

/**
 * @brief 验证资源 / Validate resources
 * 
 * 检查所有资源是否有效，无效则调用 Create() 创建 GPU 资源。
 * Check if all resources are valid, call Create() to create GPU resource if invalid.
 */
void RenderGraph::ValidateResources()
{
    for (auto& resource : m_Resources)
    {
        if (!resource->IsValid())
        {
            resource->Create();
        }
    }
}
