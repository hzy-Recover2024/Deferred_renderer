/**
 * @file Texture.cpp
 * @brief 纹理资源类实现 / Texture resource class implementation
 */

#include "Texture.h"
#include <iostream>

/**
 * @brief 构造函数实现 / Constructor implementation
 */
Texture::Texture(const std::string& name)
    : RenderResource(name, ResourceType::Texture), TextureID(0)
{
}

/**
 * @brief 带描述的构造函数实现 / Constructor with description implementation
 */
Texture::Texture(const std::string& name, const Desc& desc)
    : RenderResource(name, ResourceType::Texture), TextureID(0), m_Desc(desc)
{
}

/**
 * @brief 析构函数，自动销毁纹理 / Destructor, automatically destroys texture
 */
Texture::~Texture()
{
    Destroy();
}

/**
 * @brief 创建 GPU 纹理资源 / Create GPU texture resource
 * 
 * 创建 OpenGL 纹理对象并设置纹理参数。
 * Creates OpenGL texture object and sets texture parameters.
 */
void Texture::Create()
{
    if (TextureID != 0)
        return;

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    /// 分配纹理存储 / Allocate texture storage
    glTexImage2D(GL_TEXTURE_2D, 0, m_Desc.InternalFormat,
                 m_Desc.Width, m_Desc.Height, 0,
                 m_Desc.Format, m_Desc.Type, nullptr);

    /// 设置纹理过滤参数 / Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Desc.FilterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Desc.FilterMag);

    /// 设置纹理寻址参数 / Set texture addressing parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Desc.WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Desc.WrapT);

    /// 生成 Mipmap（如果需要）/ Generate mipmaps (if needed)
    if (m_Desc.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief 销毁 GPU 纹理资源 / Destroy GPU texture resource
 */
void Texture::Destroy()
{
    if (TextureID != 0)
    {
        glDeleteTextures(1, &TextureID);
        TextureID = 0;
    }
}

/**
 * @brief 绑定纹理到指定纹理槽 / Bind texture to specified texture slot
 * @param slot 纹理槽索引 / Texture slot index
 */
void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, TextureID);
}

/**
 * @brief 解绑纹理 / Unbind texture
 */
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief 调整纹理大小 / Resize texture
 * @param width 新宽度 / New width
 * @param height 新高度 / New height
 * 
 * 重新分配纹理存储，保留原有参数。
 * Reallocates texture storage, preserving original parameters.
 */
void Texture::Resize(unsigned int width, unsigned int height)
{
    m_Desc.Width = width;
    m_Desc.Height = height;

    if (TextureID != 0)
    {
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_Desc.InternalFormat,
                     m_Desc.Width, m_Desc.Height, 0,
                     m_Desc.Format, m_Desc.Type, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
