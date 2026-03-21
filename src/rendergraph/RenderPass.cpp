/**
 * @file RenderPass.cpp
 * @brief 渲染通道基类实现 / Render pass base class implementation
 */

#include "rendergraph/RenderPass.h"

/**
 * @brief 构造函数实现 / Constructor implementation
 */
RenderPass::RenderPass(const std::string& name)
    : Name(name), State(RenderPassState::Pending)
{
}

/**
 * @brief 添加输入资源 / Add input resource
 */
void RenderPass::AddInput(RefPtr<RenderResource> resource)
{
    if (resource)
    {
        m_Inputs.push_back(resource);
    }
}

/**
 * @brief 添加输出资源 / Add output resource
 */
void RenderPass::AddOutput(RefPtr<RenderResource> resource)
{
    if (resource)
    {
        m_Outputs.push_back(resource);
    }
}

/**
 * @brief 添加依赖的渲染通道 / Add dependent render pass
 */
void RenderPass::AddDependency(RefPtr<RenderPass> pass)
{
    if (pass)
    {
        m_Dependencies.push_back(pass);
    }
}

/**
 * @brief 准备通道执行 / Prepare pass for execution
 * 
 * 将状态设置为 Ready 并调用 Setup() 初始化资源。
 * Sets state to Ready and calls Setup() to initialize resources.
 */
void RenderPass::Prepare()
{
    State = RenderPassState::Ready;
    Setup();
}

/**
 * @brief 运行通道 / Run pass
 * 
 * 执行渲染逻辑并更新状态。
 * Executes rendering logic and updates state.
 */
void RenderPass::Run()
{
    State = RenderPassState::Executing;
    Execute();
    State = RenderPassState::Completed;
}

/**
 * @brief 完成通道清理 / Finish pass cleanup
 * 
 * 清理所有输入、输出和依赖列表，释放资源引用。
 * Clears all input, output, and dependency lists, releasing resource references.
 */
void RenderPass::Finish()
{
    m_Inputs.clear();
    m_Outputs.clear();
    m_Dependencies.clear();
}
