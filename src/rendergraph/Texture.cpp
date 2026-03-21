#include "rendergraph/Texture.h"
#include <iostream>

Texture::Texture(const std::string& name)
    : RenderResource(name, ResourceType::Texture), TextureID(0)
{
}

Texture::Texture(const std::string& name, const Desc& desc)
    : RenderResource(name, ResourceType::Texture), TextureID(0), m_Desc(desc)
{
}

Texture::~Texture()
{
    Destroy();
}

void Texture::Create()
{
    if (TextureID != 0)
        return;

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, m_Desc.InternalFormat,
                 m_Desc.Width, m_Desc.Height, 0,
                 m_Desc.Format, m_Desc.Type, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Desc.FilterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Desc.FilterMag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Desc.WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Desc.WrapT);

    if (m_Desc.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Destroy()
{
    if (TextureID != 0)
    {
        glDeleteTextures(1, &TextureID);
        TextureID = 0;
    }
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, TextureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

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
