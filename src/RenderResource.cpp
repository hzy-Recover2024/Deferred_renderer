/**
 * @file RenderResource.cpp
 * @brief 渲染资源基类实现
 */

#include "RenderResource.h"

unsigned int RenderResource::s_NextID = 1;

RenderResource::RenderResource(const std::string& name, ResourceType type)
    : Name(name), Type(type)
{
    ID = s_NextID++;
}
