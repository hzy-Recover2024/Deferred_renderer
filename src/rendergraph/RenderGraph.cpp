#include "rendergraph/RenderGraph.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <queue>

RenderGraph::RenderGraph()
{
}

void RenderGraph::AddPass(std::unique_ptr<RenderPass> pass)
{
    if (!pass)
        return;

    m_PassMap[pass->Name] = pass.get();
    m_Passes.push_back(std::move(pass));
}

void RenderGraph::Compile()
{
    if (DetectCycle())
    {
        std::cerr << "RenderGraph: Cycle detected in pass dependencies!" << std::endl;
        return;
    }

    TopologicalSort();
    ValidateResources();

    for (auto* pass : m_SortedPasses)
    {
        pass->Prepare();
    }
}

void RenderGraph::Execute()
{
    for (auto* pass : m_SortedPasses)
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
}

RenderPass* RenderGraph::GetPass(const std::string& name) const
{
    auto it = m_PassMap.find(name);
    if (it != m_PassMap.end())
        return it->second;
    return nullptr;
}

std::shared_ptr<RenderResource> RenderGraph::GetResource(const std::string& name) const
{
    auto it = m_ResourceMap.find(name);
    if (it != m_ResourceMap.end())
        return it->second;
    return nullptr;
}

void RenderGraph::TopologicalSort()
{
    m_SortedPasses.clear();

    std::unordered_map<RenderPass*, int> inDegree;
    std::unordered_map<RenderPass*, std::vector<RenderPass*>> adjacencyList;

    for (auto& pass : m_Passes)
    {
        inDegree[pass.get()] = 0;
    }

    for (auto& pass : m_Passes)
    {
        for (auto* dep : pass->GetDependencies())
        {
            adjacencyList[dep].push_back(pass.get());
            inDegree[pass.get()]++;
        }
    }

    std::queue<RenderPass*> queue;
    for (auto& pass : m_Passes)
    {
        if (inDegree[pass.get()] == 0)
        {
            queue.push(pass.get());
        }
    }

    while (!queue.empty())
    {
        RenderPass* current = queue.front();
        queue.pop();
        m_SortedPasses.push_back(current);

        for (auto* neighbor : adjacencyList[current])
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
    std::unordered_set<RenderPass*> visited;
    std::unordered_set<RenderPass*> recursionStack;

    std::function<bool(RenderPass*)> hasCycle = [&](RenderPass* pass) -> bool {
        if (recursionStack.count(pass))
            return true;

        if (visited.count(pass))
            return false;

        visited.insert(pass);
        recursionStack.insert(pass);

        for (auto* dep : pass->GetDependencies())
        {
            if (hasCycle(dep))
                return true;
        }

        recursionStack.erase(pass);
        return false;
    };

    for (auto& pass : m_Passes)
    {
        if (hasCycle(pass.get()))
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
