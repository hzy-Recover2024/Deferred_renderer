/**
 * @file ShaderManager.h
 * @brief 着色器管理器
 * 
 * 统一管理所有着色器的创建、缓存和生命周期。
 */

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "RenderResource.h"
#include "shader_shaderc.h"
#include <unordered_map>
#include <string>
#include <vector>

/**
 * @brief 着色器管理器类
 * 
 * 单例模式，统一管理所有着色器资源：
 * 1. 着色器创建和缓存
 * 2. 着色器生命周期管理
 * 3. 着色器共享
 */
class ShaderManager
{
public:
    static ShaderManager& GetInstance()
    {
        static ShaderManager instance;
        return instance;
    }

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    /**
     * @brief 加载着色器
     * 
     * 如果着色器已存在，直接返回缓存的着色器。
     */
    RefPtr<Shader> Load(const std::string& name,
                        const std::string& vertexPath,
                        const std::string& fragmentPath,
                        const std::vector<std::pair<std::string, std::string>>& defines = {});

    RefPtr<Shader> Get(const std::string& name) const;
    bool Has(const std::string& name) const;
    void Unload(const std::string& name);
    void Clear();
    size_t Count() const { return m_Shaders.size(); }

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    std::unordered_map<std::string, RefPtr<Shader>> m_Shaders;
};

#endif
