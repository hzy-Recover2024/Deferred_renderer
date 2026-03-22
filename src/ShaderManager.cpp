/**
 * @file ShaderManager.cpp
 * @brief 着色器管理器实现
 */

#include "ShaderManager.h"
#include <iostream>

RefPtr<Shader> ShaderManager::Load(const std::string& name,
                                    const std::string& vertexPath,
                                    const std::string& fragmentPath,
                                    const std::vector<std::pair<std::string, std::string>>& defines)
{
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end())
    {
        return it->second;
    }

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

RefPtr<Shader> ShaderManager::Get(const std::string& name) const
{
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end())
    {
        return it->second;
    }
    return nullptr;
}

bool ShaderManager::Has(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

void ShaderManager::Unload(const std::string& name)
{
    m_Shaders.erase(name);
}

void ShaderManager::Clear()
{
    m_Shaders.clear();
}
