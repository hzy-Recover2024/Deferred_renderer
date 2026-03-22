/**
 * @file RenderPass.h
 * @brief 渲染通道基类
 * 
 * 定义渲染通道的基类，包含输入/输出资源和依赖关系管理。
 */

#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include "RenderResource.h"
#include <string>
#include <vector>

enum class RenderPassState {
    Pending,
    Ready,
    Executing,
    Completed
};

/**
 * @brief 渲染通道基类
 * 
 * 每个渲染通道代表渲染管线中的一个阶段，如几何通道、光照通道、后处理通道等。
 * 
 * 状态流转：
 * Pending --> Ready --> Executing --> Completed
 *             │           │
 *         Prepare()    Run()
 */
class RenderPass : public std::enable_shared_from_this<RenderPass>
{
public:
    std::string Name;
    RenderPassState State;

    RenderPass(const std::string& name);
    virtual ~RenderPass() = default;

    void AddInput(RefPtr<RenderResource> resource);
    void AddOutput(RefPtr<RenderResource> resource);
    const std::vector<RefPtr<RenderResource>>& GetInputs() const { return m_Inputs; }
    const std::vector<RefPtr<RenderResource>>& GetOutputs() const { return m_Outputs; }

    /**
     * @brief 添加依赖的渲染通道
     * 
     * 当前通道将在所有依赖通道完成后执行。
     */
    void AddDependency(RefPtr<RenderPass> pass);
    const std::vector<RefPtr<RenderPass>>& GetDependencies() const { return m_Dependencies; }

    virtual void Setup() = 0;
    virtual void Execute() = 0;

    void Prepare();
    void Run();
    void Finish();

protected:
    std::vector<RefPtr<RenderResource>> m_Inputs;
    std::vector<RefPtr<RenderResource>> m_Outputs;
    std::vector<RefPtr<RenderPass>> m_Dependencies;
};

#endif
