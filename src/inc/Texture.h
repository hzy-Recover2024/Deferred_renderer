/**
 * @file Texture.h
 * @brief 纹理资源类定义 / Texture resource class definition
 * 
 * 封装 OpenGL 纹理对象，提供纹理创建、绑定、调整大小等功能。
 * Encapsulates OpenGL texture object, providing texture creation, 
 * binding, resizing and other functions.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "RenderResource.h"
#include <glad/glad.h>

/**
 * @brief 纹理资源类 / Texture resource class
 * 
 * 继承自 RenderResource，管理 OpenGL 纹理对象的生命周期。
 * Inherits from RenderResource, manages lifecycle of OpenGL texture object.
 * 
 * 支持功能 / Supported features:
 * - 多种纹理格式 / Various texture formats
 * - 可配置的过滤和寻址模式 / Configurable filtering and addressing modes
 * - 动态调整大小 / Dynamic resizing
 * - Mipmap 生成 / Mipmap generation
 */
class Texture : public RenderResource
{
public:
    /**
     * @brief 纹理描述结构体 / Texture description structure
     * 
     * 定义纹理的各种属性。
     * Defines various properties of a texture.
     */
    struct Desc {
        unsigned int Width = 0;           ///< 纹理宽度 / Texture width
        unsigned int Height = 0;          ///< 纹理高度 / Texture height
        GLenum InternalFormat = GL_RGBA8; ///< 内部格式 / Internal format
        GLenum Format = GL_RGBA;          ///< 像素数据格式 / Pixel data format
        GLenum Type = GL_UNSIGNED_BYTE;   ///< 像素数据类型 / Pixel data type
        GLenum FilterMin = GL_LINEAR;     ///< 缩小过滤模式 / Minification filter mode
        GLenum FilterMag = GL_LINEAR;     ///< 放大过滤模式 / Magnification filter mode
        GLenum WrapS = GL_CLAMP_TO_EDGE;  ///< S 方向寻址模式 / S-axis wrap mode
        GLenum WrapT = GL_CLAMP_TO_EDGE;  ///< T 方向寻址模式 / T-axis wrap mode
        bool GenerateMipmaps = false;     ///< 是否生成 Mipmap / Whether to generate mipmaps
    };

    unsigned int TextureID;  ///< OpenGL 纹理 ID / OpenGL texture ID

    /**
     * @brief 构造函数 / Constructor
     * @param name 纹理名称 / Texture name
     */
    Texture(const std::string& name);

    /**
     * @brief 带描述的构造函数 / Constructor with description
     * @param name 纹理名称 / Texture name
     * @param desc 纹理描述 / Texture description
     */
    Texture(const std::string& name, const Desc& desc);

    /**
     * @brief 析构函数，自动销毁纹理 / Destructor, automatically destroys texture
     */
    ~Texture();

    /**
     * @brief 创建 GPU 纹理资源 / Create GPU texture resource
     * 
     * 调用 glGenTextures 创建纹理对象并设置参数。
     * Calls glGenTextures to create texture object and set parameters.
     */
    void Create() override;

    /**
     * @brief 销毁 GPU 纹理资源 / Destroy GPU texture resource
     * 
     * 调用 glDeleteTextures 删除纹理对象。
     * Calls glDeleteTextures to delete texture object.
     */
    void Destroy() override;

    /**
     * @brief 检查纹理是否有效 / Check if texture is valid
     * @return true 纹理 ID 非 0 / Texture ID is non-zero
     * @return false 纹理 ID 为 0 / Texture ID is zero
     */
    bool IsValid() const override { return TextureID != 0; }

    /**
     * @brief 绑定纹理到指定纹理槽 / Bind texture to specified texture slot
     * @param slot 纹理槽索引，默认为 0 / Texture slot index, default is 0
     */
    void Bind(unsigned int slot = 0) const;

    /**
     * @brief 解绑纹理 / Unbind texture
     */
    void Unbind() const;

    /**
     * @brief 调整纹理大小 / Resize texture
     * @param width 新宽度 / New width
     * @param height 新高度 / New height
     * 
     * 销毁旧纹理并创建新尺寸的纹理。
     * Destroys old texture and creates new texture with new dimensions.
     */
    void Resize(unsigned int width, unsigned int height);

    /**
     * @brief 获取纹理宽度 / Get texture width
     * @return 宽度 / Width
     */
    unsigned int GetWidth() const { return m_Desc.Width; }

    /**
     * @brief 获取纹理高度 / Get texture height
     * @return 高度 / Height
     */
    unsigned int GetHeight() const { return m_Desc.Height; }

    /**
     * @brief 获取纹理描述 / Get texture description
     * @return 纹理描述结构体 / Texture description structure
     */
    const Desc& GetDesc() const { return m_Desc; }

private:
    Desc m_Desc;  ///< 纹理描述 / Texture description
};

#endif
