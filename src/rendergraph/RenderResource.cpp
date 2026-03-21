/**
 * @file RenderResource.cpp
 * @brief 渲染资源基类实现 / Render resource base class implementation
 */

#include "rendergraph/RenderResource.h"

/// 静态成员初始化：下一个可用 ID / Static member initialization: next available ID
unsigned int RenderResource::s_NextID = 1;

/**
 * @brief 构造函数实现 / Constructor implementation
 */
RenderResource::RenderResource(const std::string& name, ResourceType type)
    : Name(name), Type(type)
{
    ID = s_NextID++;
}
