/**
 * @file RenderGraph.cpp
 * @brief 渲染图实现
 */

#include "RenderGraph.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <queue>

RenderGraph::RenderGraph()
{
}

void RenderGraph::Compile()
{
    if (!m_NeedsRecompile && m_IsCompiled)
        return;

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

void RenderGraph::ForceRecompile()
{
    m_NeedsRecompile = true;
    m_IsCompiled = false;
    Compile();
}

void RenderGraph::Execute()
{
    if (!m_IsCompiled)
        Compile();

    for (auto& pass : m_SortedPasses)
    {
        pass->Run();
    }
}

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

    m_NeedsRecompile = true;
    m_IsCompiled = false;
}

RefPtr<RenderPass> RenderGraph::GetPass(const std::string& name) const
{
    auto it = m_PassMap.find(name);
    if (it != m_PassMap.end())
        return it->second;
    return nullptr;
}

RefPtr<RenderResource> RenderGraph::GetResource(const std::string& name) const
{
    auto it = m_ResourceMap.find(name);
    if (it != m_ResourceMap.end())
        return it->second;
    return nullptr;
}

void RenderGraph::TopologicalSort()
{
    m_SortedPasses.clear();

    std::unordered_map<RefPtr<RenderPass>, int> inDegree;
    std::unordered_map<RefPtr<RenderPass>, std::vector<RefPtr<RenderPass>>> adjacencyList;

    for (auto& pass : m_Passes)
    {
        inDegree[pass] = 0;
    }

    for (auto& pass : m_Passes)
    {
        for (auto& dep : pass->GetDependencies())
        {
            adjacencyList[dep].push_back(pass);
            inDegree[pass]++;
        }
    }

    std::queue<RefPtr<RenderPass>> queue;
    for (auto& pass : m_Passes)
    {
        if (inDegree[pass] == 0)
        {
            queue.push(pass);
        }
    }

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

bool RenderGraph::DetectCycle()
{
    std::unordered_set<RefPtr<RenderPass>> visited;
    std::unordered_set<RefPtr<RenderPass>> recursionStack;

    std::function<bool(RefPtr<RenderPass>)> hasCycle = [&](RefPtr<RenderPass> pass) -> bool {
        if (recursionStack.count(pass))
            return true;

        if (visited.count(pass))
            return false;

        visited.insert(pass);
        recursionStack.insert(pass);

        for (auto& dep : pass->GetDependencies())
        {
            if (hasCycle(dep))
                return true;
        }

        recursionStack.erase(pass);
        return false;
    };

    for (auto& pass : m_Passes)
    {
        if (hasCycle(pass))
            return true;
    }

    return false;
}

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
