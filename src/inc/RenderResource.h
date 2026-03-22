/**
 * @file RenderResource.h
 * @brief 渲染资源基类
 * 
 * 所有渲染资源（纹理、缓冲、帧缓冲等）的基类，提供统一的资源管理接口。
 */

#ifndef RENDER_RESOURCE_H
#define RENDER_RESOURCE_H

#include <string>
#include <memory>

enum class ResourceType {
    Texture,
    Buffer,
    Framebuffer,
    Unknown
};

/**
 * @brief 渲染资源基类
 * 
 * 定义资源的基本属性和生命周期管理接口：
 * 1. 构造函数初始化名称和类型
 * 2. Create() 创建 GPU 资源
 * 3. 使用资源
 * 4. Destroy() 销毁 GPU 资源
 */
class RenderResource
{
public:
    std::string Name;
    ResourceType Type;
    unsigned int ID;

    RenderResource(const std::string& name, ResourceType type);
    virtual ~RenderResource() = default;

    virtual void Create() = 0;
    virtual void Destroy() = 0;
    virtual bool IsValid() const = 0;

protected:
    static unsigned int s_NextID;
};

template<typename T>
using RefPtr = std::shared_ptr<T>;

template<typename T, typename... Args>
RefPtr<T> MakeRefPtr(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif
