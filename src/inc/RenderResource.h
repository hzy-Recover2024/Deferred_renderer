/**
 * @file RenderResource.h
 * @brief 渲染资源基类定义 / Render resource base class definition
 * 
 * 所有渲染资源（纹理、帧缓冲等）的基类，提供统一的资源管理接口。
 * Base class for all render resources (textures, framebuffers, etc.), 
 * providing unified resource management interface.
 */

#ifndef RENDER_RESOURCE_H
#define RENDER_RESOURCE_H

#include <string>
#include <memory>

/**
 * @brief 资源类型枚举 / Resource type enumeration
 */
enum class ResourceType {
    Texture,      ///< 纹理资源 / Texture resource
    Buffer,       ///< 缓冲资源 / Buffer resource
    Framebuffer,  ///< 帧缓冲资源 / Framebuffer resource
    Unknown       ///< 未知类型 / Unknown type
};

/**
 * @brief 渲染资源基类 / Render resource base class
 * 
 * 所有渲染资源的抽象基类，定义了资源的基本属性和生命周期管理接口。
 * Abstract base class for all render resources, defining basic properties 
 * and lifecycle management interface.
 * 
 * 生命周期 / Lifecycle:
 * 1. 构造函数初始化名称和类型 / Constructor initializes name and type
 * 2. Create() 创建 GPU 资源 / Create() creates GPU resource
 * 3. 使用资源 / Use resource
 * 4. Destroy() 销毁 GPU 资源 / Destroy() destroys GPU resource
 */
class RenderResource
{
public:
    std::string Name;        ///< 资源名称，用于查找和调试 / Resource name for lookup and debugging
    ResourceType Type;       ///< 资源类型 / Resource type
    unsigned int ID;         ///< 唯一标识符 / Unique identifier

    /**
     * @brief 构造函数 / Constructor
     * @param name 资源名称 / Resource name
     * @param type 资源类型 / Resource type
     */
    RenderResource(const std::string& name, ResourceType type);

    /**
     * @brief 虚析构函数 / Virtual destructor
     */
    virtual ~RenderResource() = default;

    /**
     * @brief 创建 GPU 资源 / Create GPU resource
     * 
     * 在 GPU 上分配资源，如生成纹理对象、帧缓冲对象等。
     * Allocate resource on GPU, such as generating texture objects, framebuffer objects, etc.
     */
    virtual void Create() = 0;

    /**
     * @brief 销毁 GPU 资源 / Destroy GPU resource
     * 
     * 释放 GPU 上的资源，如删除纹理对象、帧缓冲对象等。
     * Release resource on GPU, such as deleting texture objects, framebuffer objects, etc.
     */
    virtual void Destroy() = 0;

    /**
     * @brief 检查资源是否有效 / Check if resource is valid
     * @return true 资源有效 / Resource is valid
     * @return false 资源无效 / Resource is invalid
     */
    virtual bool IsValid() const = 0;

protected:
    static unsigned int s_NextID;  ///< 下一个可用 ID / Next available ID
};

/**
 * @brief 引用计数智能指针模板 / Reference counted smart pointer template
 * 
 * 用于管理渲染资源的生命周期，基于 std::shared_ptr 实现。
 * Used to manage lifecycle of render resources, based on std::shared_ptr.
 */
template<typename T>
using RefPtr = std::shared_ptr<T>;

/**
 * @brief 创建引用计数智能指针 / Create reference counted smart pointer
 * 
 * 工厂函数，用于创建 RefPtr 管理的对象。
 * Factory function to create RefPtr-managed objects.
 * 
 * @tparam T 对象类型 / Object type
 * @tparam Args 构造函数参数类型 / Constructor argument types
 * @param args 构造函数参数 / Constructor arguments
 * @return RefPtr<T> 智能指针 / Smart pointer
 */
template<typename T, typename... Args>
RefPtr<T> MakeRefPtr(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#endif
