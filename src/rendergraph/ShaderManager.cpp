/**
 * @file ShaderManager.cpp
 * @brief 着色器管理器类实现 / Shader manager class implementation
 */

#include "rendergraph/ShaderManager.h"
#include <iostream>

/**
 * @brief 加载着色器 / Load shader
 */
RefPtr<Shader> ShaderManager::Load(const std::string& name, 
                                    const std::string& vertexPath, 
                                    const std::string& fragmentPath,
                                    const std::vector<std::pair<std::string, std::string>>& defines)
{
    /// 检查是否已缓存 / Check if already cached
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end())
    {
        return it->second;
    }

    /// 创建新着色器 / Create new shader
    try
    {
        auto shader = MakeRefPtr<Shader>(vertexPath.c_str(), fragmentPath.c_str(), defines);
        m_Shaders[name] = shader;
        return shader;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ShaderManager: Failed to load shader '" << name << "': " << e.what() << std::endl;
        return nullptr;
    }
}

/**
 * @brief 获取着色器 / Get shader
 */
RefPtr<Shader> ShaderManager::Get(const std::string& name) const
{
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end())
    {
        return it->second;
    }
    return nullptr;
}

/**
 * @brief 检查着色器是否存在 / Check if shader exists
 */
bool ShaderManager::Has(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

/**
 * @brief 卸载着色器 / Unload shader
 */
void ShaderManager::Unload(const std::string& name)
{
    m_Shaders.erase(name);
}

/**
 * @brief 卸载所有着色器 / Unload all shaders
 */
void ShaderManager::Clear()
{
    m_Shaders.clear();
}
