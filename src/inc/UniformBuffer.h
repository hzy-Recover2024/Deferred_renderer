/**
 * @file UniformBuffer.h
 * @brief Uniform 缓冲资源类定义 / Uniform buffer resource class definition
 * 
 * 封装 OpenGL Uniform Buffer Object (UBO)，支持 std140 布局。
 * Encapsulates OpenGL Uniform Buffer Object (UBO), supporting std140 layout.
 */

#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "RenderResource.h"
#include <glad/glad.h>
#include <cstddef>

/**
 * @brief Uniform 缓冲资源类 / Uniform buffer resource class
 * 
 * 继承自 RenderResource，管理 OpenGL UBO 的生命周期。
 * Inherits from RenderResource, manages lifecycle of OpenGL UBO.
 * 
 * 特性 / Features:
 * - std140 布局兼容 / std140 layout compatible
 * - 部分更新支持 / Partial update support
 * - 绑定点配置 / Binding point configuration
 * - 动态使用模式 / Dynamic usage pattern
 * 
 * 使用示例 / Usage example:
 * @code
 * // 创建 UBO
 * auto ubo = MakeRefPtr<UniformBuffer>("CameraBuffer", sizeof(CameraData), 0);
 * ubo->Create();
 * 
 * // 更新数据
 * CameraData data;
 * data.viewMatrix = camera.GetViewMatrix();
 * ubo->SetData(&data, sizeof(CameraData));
 * 
 * // Shader 中自动绑定
 * layout(std140, binding = 0) uniform CameraBlock { ... };
 * @endcode
 */
class UniformBuffer : public RenderResource
{
public:
    unsigned int UBO;  ///< OpenGL UBO ID / OpenGL UBO ID

    /**
     * @brief 构造函数 / Constructor
     * @param name 缓冲名称 / Buffer name
     * @param size 缓冲大小（字节）/ Buffer size in bytes
     * @param bindingPoint 绑定点索引 / Binding point index
     */
    UniformBuffer(const std::string& name, size_t size, unsigned int bindingPoint);

    /**
     * @brief 析构函数，自动销毁缓冲 / Destructor, automatically destroys buffer
     */
    ~UniformBuffer();

    /**
     * @brief 创建 GPU 缓冲资源 / Create GPU buffer resource
     */
    void Create() override;

    /**
     * @brief 销毁 GPU 缓冲资源 / Destroy GPU buffer resource
     */
    void Destroy() override;

    /**
     * @brief 检查缓冲是否有效 / Check if buffer is valid
     * @return true UBO 非 0 / UBO is non-zero
     * @return false UBO 为 0 / UBO is zero
     */
    bool IsValid() const override { return UBO != 0; }

    /**
     * @brief 绑定缓冲 / Bind buffer
     */
    void Bind() const;

    /**
     * @brief 解绑缓冲 / Unbind buffer
     */
    void Unbind() const;

    /**
     * @brief 设置缓冲数据 / Set buffer data
     * @param data 数据指针 / Data pointer
     * @param size 数据大小（字节）/ Data size in bytes
     * @param offset 缓冲内偏移（字节），默认为 0 / Buffer offset in bytes, default is 0
     */
    void SetData(const void* data, size_t size, size_t offset = 0);

    /**
     * @brief 绑定缓冲范围 / Bind buffer range
     * 
     * 将缓冲的一部分绑定到绑定点。
     * Binds a portion of the buffer to the binding point.
     * 
     * @param offset 偏移量（字节）/ Offset in bytes
     * @param size 大小（字节）/ Size in bytes
     */
    void BindRange(size_t offset, size_t size);

    /**
     * @brief 获取缓冲大小 / Get buffer size
     * @return 大小（字节）/ Size in bytes
     */
    size_t GetSize() const { return m_Size; }

    /**
     * @brief 获取绑定点 / Get binding point
     * @return 绑定点索引 / Binding point index
     */
    unsigned int GetBindingPoint() const { return m_BindingPoint; }

private:
    size_t m_Size;              ///< 缓冲大小 / Buffer size
    unsigned int m_BindingPoint; ///< 绑定点 / Binding point
};

#endif
