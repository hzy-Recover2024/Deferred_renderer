/**
 * @file Texture.h
 * @brief 纹理资源类
 * 
 * 封装 OpenGL 纹理对象，提供纹理创建、绑定、调整大小等功能。
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "RenderResource.h"
#include <glad/glad.h>

/**
 * @brief 纹理资源类
 * 
 * 管理纹理对象的生命周期，支持：
 * - 多种纹理格式
 * - 可配置的过滤和寻址模式
 * - 动态调整大小
 * - Mipmap 生成
 */
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

    /**
     * @brief 调整纹理大小
     * 
     * 销毁旧纹理并创建新尺寸的纹理。
     */
    void Resize(unsigned int width, unsigned int height);

    unsigned int GetWidth() const { return m_Desc.Width; }
    unsigned int GetHeight() const { return m_Desc.Height; }
    const Desc& GetDesc() const { return m_Desc; }

private:
    Desc m_Desc;
};

#endif
