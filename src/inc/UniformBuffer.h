/**
 * @file UniformBuffer.h
 * @brief Uniform 缓冲资源类
 */

#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "RenderResource.h"
#include <glad/glad.h>
#include <cstddef>

class UniformBuffer : public RenderResource
{
public:
    unsigned int UBO;

    UniformBuffer(const std::string& name, size_t size, unsigned int bindingPoint);
    ~UniformBuffer();

    void Create() override;
    void Destroy() override;
    bool IsValid() const override { return UBO != 0; }

    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, size_t size, size_t offset = 0);
    void BindRange(size_t offset, size_t size);

    size_t GetSize() const { return m_Size; }
    unsigned int GetBindingPoint() const { return m_BindingPoint; }

private:
    size_t m_Size;
    unsigned int m_BindingPoint;
};

#endif
