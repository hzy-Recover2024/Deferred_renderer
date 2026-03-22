/**
 * @file UniformBuffer.cpp
 * @brief Uniform 缓冲资源类实现 / Uniform buffer resource class implementation
 */

#include "UniformBuffer.h"
#include <iostream>

UniformBuffer::UniformBuffer(const std::string& name, size_t size, unsigned int bindingPoint)
    : RenderResource(name, ResourceType::Buffer)
    , UBO(0)
    , m_Size(size)
    , m_BindingPoint(bindingPoint)
{
}

UniformBuffer::~UniformBuffer()
{
    if (IsValid())
    {
        Destroy();
    }
}

void UniformBuffer::Create()
{
    if (IsValid())
    {
        return;
    }

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Destroy()
{
    if (!IsValid())
    {
        return;
    }

    glDeleteBuffers(1, &UBO);
    UBO = 0;
}

void UniformBuffer::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

void UniformBuffer::Unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetData(const void* data, size_t size, size_t offset)
{
    if (!IsValid() || data == nullptr || size == 0)
    {
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::BindRange(size_t offset, size_t size)
{
    if (!IsValid())
    {
        return;
    }

    glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingPoint, UBO, offset, size);
}
