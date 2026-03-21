/**
 * @file RenderPass.h
 * @brief 渲染通道基类定义 / Render pass base class definition
 * 
 * 定义渲染通道的基类，包含输入/输出资源和依赖关系管理。
 * Defines the base class for render passes, including input/output resources 
 * and dependency management.
 */

#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include "RenderResource.h"
#include <string>
#include <vector>

/**
 * @brief 渲染通道状态枚举 / Render pass state enumeration
 */
enum class RenderPassState {
    Pending,    ///< 等待初始化 / Waiting for initialization
    Ready,      ///< 已准备就绪 / Ready to execute
    Executing,  ///< 正在执行 / Currently executing
    Completed   ///< 已完成 / Execution completed
};

/**
 * @brief 渲染通道基类 / Render pass base class
 * 
 * 抽象基类，定义渲染通道的基本结构。每个渲染通道代表渲染管线中的一个阶段，
 * 如几何通道、光照通道、后处理通道等。
 * Abstract base class defining the basic structure of a render pass.
 * Each render pass represents a stage in the rendering pipeline,
 * such as geometry pass, lighting pass, post-processing pass, etc.
 * 
 * 状态流转 / State flow:
 * Pending --> Ready --> Executing --> Completed
 *             │           │
 *         Prepare()    Run()
 * 
 * 使用方式 / Usage:
 * 1. 继承此类并实现 Setup() 和 Execute() / Inherit this class and implement Setup() and Execute()
 * 2. 在 Setup() 中初始化资源 / Initialize resources in Setup()
 * 3. 在 Execute() 中执行渲染逻辑 / Execute rendering logic in Execute()
 */
class RenderPass : public std::enable_shared_from_this<RenderPass>
{
public:
    std::string Name;        ///< 通道名称 / Pass name
    RenderPassState State;   ///< 当前状态 / Current state

    /**
     * @brief 构造函数 / Constructor
     * @param name 通道名称 / Pass name
     */
    RenderPass(const std::string& name);

    /**
     * @brief 虚析构函数 / Virtual destructor
     */
    virtual ~RenderPass() = default;

    /**
     * @brief 添加输入资源 / Add input resource
     * @param resource 输入资源指针 / Input resource pointer
     */
    void AddInput(RefPtr<RenderResource> resource);

    /**
     * @brief 添加输出资源 / Add output resource
     * @param resource 输出资源指针 / Output resource pointer
     */
    void AddOutput(RefPtr<RenderResource> resource);

    /**
     * @brief 获取所有输入资源 / Get all input resources
     * @return 输入资源列表 / List of input resources
     */
    const std::vector<RefPtr<RenderResource>>& GetInputs() const { return m_Inputs; }

    /**
     * @brief 获取所有输出资源 / Get all output resources
     * @return 输出资源列表 / List of output resources
     */
    const std::vector<RefPtr<RenderResource>>& GetOutputs() const { return m_Outputs; }

    /**
     * @brief 添加依赖的渲染通道 / Add dependent render pass
     * @param pass 依赖的通道 / Dependent pass
     * 
     * 当前通道将在所有依赖通道完成后执行。
     * Current pass will execute after all dependent passes complete.
     */
    void AddDependency(RefPtr<RenderPass> pass);

    /**
     * @brief 获取所有依赖的渲染通道 / Get all dependent render passes
     * @return 依赖通道列表 / List of dependent passes
     */
    const std::vector<RefPtr<RenderPass>>& GetDependencies() const { return m_Dependencies; }

    /**
     * @brief 设置通道（纯虚函数）/ Setup pass (pure virtual)
     * 
     * 在此方法中初始化通道所需的资源，如创建帧缓冲、设置着色器等。
     * Initialize resources needed by the pass in this method,
     * such as creating framebuffers, setting up shaders, etc.
     */
    virtual void Setup() = 0;

    /**
     * @brief 执行渲染（纯虚函数）/ Execute rendering (pure virtual)
     * 
     * 在此方法中实现实际的渲染逻辑。
     * Implement actual rendering logic in this method.
     */
    virtual void Execute() = 0;

    /**
     * @brief 准备通道执行 / Prepare pass for execution
     * 
     * 将状态设置为 Ready 并调用 Setup()。
     * Sets state to Ready and calls Setup().
     */
    void Prepare();

    /**
     * @brief 运行通道 / Run pass
     * 
     * 将状态设置为 Executing，调用 Execute()，然后设置为 Completed。
     * Sets state to Executing, calls Execute(), then sets to Completed.
     */
    void Run();

    /**
     * @brief 完成通道清理 / Finish pass cleanup
     * 
     * 清理输入、输出和依赖列表。
     * Clears input, output, and dependency lists.
     */
    void Finish();

protected:
    std::vector<RefPtr<RenderResource>> m_Inputs;   ///< 输入资源列表 / Input resource list
    std::vector<RefPtr<RenderResource>> m_Outputs;  ///< 输出资源列表 / Output resource list
    std::vector<RefPtr<RenderPass>> m_Dependencies; ///< 依赖通道列表 / Dependent pass list
};

#endif
