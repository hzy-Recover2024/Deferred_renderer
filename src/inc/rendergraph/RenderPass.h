#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include "RenderResource.h"
#include <string>
#include <vector>
#include <memory>

enum class RenderPassState {
    Pending,
    Ready,
    Executing,
    Completed
};

class RenderPass
{
public:
    std::string Name;
    RenderPassState State;

    RenderPass(const std::string& name);
    virtual ~RenderPass() = default;

    void AddInput(std::shared_ptr<RenderResource> resource);
    void AddOutput(std::shared_ptr<RenderResource> resource);

    const std::vector<std::shared_ptr<RenderResource>>& GetInputs() const { return m_Inputs; }
    const std::vector<std::shared_ptr<RenderResource>>& GetOutputs() const { return m_Outputs; }

    void AddDependency(RenderPass* pass);
    const std::vector<RenderPass*>& GetDependencies() const { return m_Dependencies; }

    virtual void Setup() = 0;
    virtual void Execute() = 0;

    void Prepare();
    void Run();
    void Finish();

protected:
    std::vector<std::shared_ptr<RenderResource>> m_Inputs;
    std::vector<std::shared_ptr<RenderResource>> m_Outputs;
    std::vector<RenderPass*> m_Dependencies;
};

#endif
