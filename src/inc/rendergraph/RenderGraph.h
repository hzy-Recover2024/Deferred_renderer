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

class RenderGraph
{
public:
    RenderGraph();
    ~RenderGraph() = default;

    template<typename T, typename... Args>
    std::shared_ptr<T> CreateResource(Args&&... args)
    {
        auto resource = std::make_shared<T>(std::forward<Args>(args)...);
        m_Resources.push_back(resource);
        m_ResourceMap[resource->Name] = resource;
        return resource;
    }

    void AddPass(std::unique_ptr<RenderPass> pass);

    void Compile();
    void Execute();
    void Clear();

    RenderPass* GetPass(const std::string& name) const;
    std::shared_ptr<RenderResource> GetResource(const std::string& name) const;

    void SetBackbufferWidth(unsigned int width) { m_BackbufferWidth = width; }
    void SetBackbufferHeight(unsigned int height) { m_BackbufferHeight = height; }

    unsigned int GetBackbufferWidth() const { return m_BackbufferWidth; }
    unsigned int GetBackbufferHeight() const { return m_BackbufferHeight; }

private:
    void TopologicalSort();
    bool DetectCycle();
    void ValidateResources();

    std::vector<std::unique_ptr<RenderPass>> m_Passes;
    std::vector<std::shared_ptr<RenderResource>> m_Resources;
    std::unordered_map<std::string, RenderPass*> m_PassMap;
    std::unordered_map<std::string, std::shared_ptr<RenderResource>> m_ResourceMap;

    std::vector<RenderPass*> m_SortedPasses;

    unsigned int m_BackbufferWidth = 800;
    unsigned int m_BackbufferHeight = 600;
};

#endif
