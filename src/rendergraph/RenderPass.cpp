#include "rendergraph/RenderPass.h"

RenderPass::RenderPass(const std::string& name)
    : Name(name), State(RenderPassState::Pending)
{
}

void RenderPass::AddInput(std::shared_ptr<RenderResource> resource)
{
    if (resource)
    {
        m_Inputs.push_back(resource);
        resource->AddRef();
    }
}

void RenderPass::AddOutput(std::shared_ptr<RenderResource> resource)
{
    if (resource)
    {
        m_Outputs.push_back(resource);
        resource->AddRef();
    }
}

void RenderPass::AddDependency(RenderPass* pass)
{
    if (pass)
    {
        m_Dependencies.push_back(pass);
    }
}

void RenderPass::Prepare()
{
    State = RenderPassState::Ready;
    Setup();
}

void RenderPass::Run()
{
    State = RenderPassState::Executing;
    Execute();
    State = RenderPassState::Completed;
}

void RenderPass::Finish()
{
    for (auto& input : m_Inputs)
    {
        if (input)
            input->Release();
    }
    for (auto& output : m_Outputs)
    {
        if (output)
            output->Release();
    }
    m_Inputs.clear();
    m_Outputs.clear();
    m_Dependencies.clear();
}
