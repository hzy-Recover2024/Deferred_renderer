/**
 * @file RenderPass.cpp
 * @brief 渲染通道基类实现
 */

#include "RenderPass.h"

RenderPass::RenderPass(const std::string& name)
    : Name(name), State(RenderPassState::Pending)
{
}

void RenderPass::AddInput(RefPtr<RenderResource> resource)
{
    if (resource)
    {
        m_Inputs.push_back(resource);
    }
}

void RenderPass::AddOutput(RefPtr<RenderResource> resource)
{
    if (resource)
    {
        m_Outputs.push_back(resource);
    }
}

void RenderPass::AddDependency(RefPtr<RenderPass> pass)
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
    m_Inputs.clear();
    m_Outputs.clear();
    m_Dependencies.clear();
}
