/**
 * @file ShaderManager.h
 * @brief 着色器管理器类定义 / Shader manager class definition
 * 
 * 统一管理所有着色器的创建、缓存和生命周期。
 * Unified management of shader creation, caching, and lifecycle.
 */

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "RenderResource.h"
#include "shader_shaderc.h"
#include <unordered_map>
#include <string>
#include <vector>

/**
 * @brief 着色器管理器类 / Shader manager class
 * 
 * 单例模式，统一管理所有着色器资源。
 * Singleton pattern, unified management of all shader resources.
 * 
 * 职责 / Responsibilities:
 * 1. 着色器创建和缓存 / Shader creation and caching
 * 2. 着色器生命周期管理 / Shader lifecycle management
 * 3. 着色器共享 / Shader sharing
 * 
 * 使用示例 / Usage example:
 * @code
 * auto& manager = ShaderManager::GetInstance();
 * 
 * // 加载着色器 / Load shader
 * auto shader = manager.Load("geometry", "shader/deferred/gbuffer.vs", "shader/deferred/gbuffer.fs");
 * 
 * // 获取着色器 / Get shader
 * auto shader = manager.Get("geometry");
 * 
 * // 使用着色器 / Use shader
 * shader->use();
 * @endcode
 */
class ShaderManager
{
public:
    /**
     * @brief 获取单例实例 / Get singleton instance
     * @return ShaderManager& 实例引用 / Instance reference
     */
    static ShaderManager& GetInstance()
    {
        static ShaderManager instance;
        return instance;
    }

    /// 禁止拷贝 / Disable copy
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    /**
     * @brief 加载着色器 / Load shader
     * 
     * 如果着色器已存在，直接返回缓存的着色器。
     * If shader already exists, returns cached shader directly.
     * 
     * @param name 着色器名称 / Shader name
     * @param vertexPath 顶点着色器路径 / Vertex shader path
     * @param fragmentPath 片段着色器路径 / Fragment shader path
     * @param defines 预处理宏定义 / Preprocessor macro definitions
     * @return RefPtr<Shader> 着色器智能指针 / Shader smart pointer
     */
    RefPtr<Shader> Load(const std::string& name, 
                        const std::string& vertexPath, 
                        const std::string& fragmentPath,
                        const std::vector<std::pair<std::string, std::string>>& defines = {});

    /**
     * @brief 获取着色器 / Get shader
     * @param name 着色器名称 / Shader name
     * @return RefPtr<Shader> 着色器智能指针，未找到返回 nullptr / Shader smart pointer, nullptr if not found
     */
    RefPtr<Shader> Get(const std::string& name) const;

    /**
     * @brief 检查着色器是否存在 / Check if shader exists
     * @param name 着色器名称 / Shader name
     * @return true 存在 / Exists
     * @return false 不存在 / Does not exist
     */
    bool Has(const std::string& name) const;

    /**
     * @brief 卸载着色器 / Unload shader
     * @param name 着色器名称 / Shader name
     */
    void Unload(const std::string& name);

    /**
     * @brief 卸载所有着色器 / Unload all shaders
     */
    void Clear();

    /**
     * @brief 获取着色器数量 / Get shader count
     * @return 着色器数量 / Shader count
     */
    size_t Count() const { return m_Shaders.size(); }

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    /// 着色器缓存 / Shader cache
    std::unordered_map<std::string, RefPtr<Shader>> m_Shaders;
};

#endif
