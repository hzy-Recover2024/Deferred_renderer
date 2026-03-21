#ifndef TEXTURE_H
#define TEXTURE_H

#include "RenderResource.h"
#include <glad/glad.h>

class Texture : public RenderResource
{
public:
    struct Desc {
        unsigned int Width = 0;
        unsigned int Height = 0;
        GLenum InternalFormat = GL_RGBA8;
        GLenum Format = GL_RGBA;
        GLenum Type = GL_UNSIGNED_BYTE;
        GLenum FilterMin = GL_LINEAR;
        GLenum FilterMag = GL_LINEAR;
        GLenum WrapS = GL_CLAMP_TO_EDGE;
        GLenum WrapT = GL_CLAMP_TO_EDGE;
        bool GenerateMipmaps = false;
    };

    unsigned int TextureID;

    Texture(const std::string& name);
    Texture(const std::string& name, const Desc& desc);
    ~Texture();

    void Create() override;
    void Destroy() override;
    bool IsValid() const override { return TextureID != 0; }

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    void Resize(unsigned int width, unsigned int height);

    unsigned int GetWidth() const { return m_Desc.Width; }
    unsigned int GetHeight() const { return m_Desc.Height; }
    const Desc& GetDesc() const { return m_Desc; }

private:
    Desc m_Desc;
};

#endif
