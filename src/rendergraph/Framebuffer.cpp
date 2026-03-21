/**
 * @file Framebuffer.cpp
 * @brief 帧缓冲资源类实现 / Framebuffer resource class implementation
 */

#include "rendergraph/Framebuffer.h"
#include <iostream>

/**
 * @brief 构造函数实现 / Constructor implementation
 */
Framebuffer::Framebuffer(const std::string& name)
    : RenderResource(name, ResourceType::Framebuffer), FBO(0)
{
}

/**
 * @brief 析构函数，自动销毁帧缓冲 / Destructor, automatically destroys framebuffer
 */
Framebuffer::~Framebuffer()
{
    Destroy();
}

/**
 * @brief 创建 GPU 帧缓冲资源 / Create GPU framebuffer resource
 */
void Framebuffer::Create()
{
    if (FBO != 0)
        return;

    glGenFramebuffers(1, &FBO);
}

/**
 * @brief 销毁 GPU 帧缓冲资源 / Destroy GPU framebuffer resource
 */
void Framebuffer::Destroy()
{
    if (FBO != 0)
    {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
}

/**
 * @brief 添加颜色附件 / Add color attachment
 * @param texture 颜色纹理 / Color texture
 * @param index 附件索引 / Attachment index
 * 
 * 将纹理附加到帧缓冲的指定颜色附件点。
 * Attaches texture to the specified color attachment point of the framebuffer.
 */
void Framebuffer::AddColorAttachment(RefPtr<Texture> texture, unsigned int index)
{
    if (!texture)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture->Create();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->TextureID, 0);

    ColorAttachment attachment;
    attachment.texture = texture;
    attachment.Index = index;
    m_ColorAttachments.push_back(attachment);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 设置深度附件 / Set depth attachment
 * @param texture 深度纹理 / Depth texture
 */
void Framebuffer::SetDepthAttachment(RefPtr<Texture> texture)
{
    if (!texture)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture->Create();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID, 0);
    m_DepthAttachment = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 设置深度模板附件 / Set depth-stencil attachment
 * @param texture 深度模板纹理 / Depth-stencil texture
 */
void Framebuffer::SetDepthStencilAttachment(RefPtr<Texture> texture)
{
    if (!texture)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture->Create();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID, 0);
    m_DepthStencilAttachment = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 绑定帧缓冲 / Bind framebuffer
 */
void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

/**
 * @brief 解绑帧缓冲（绑定默认帧缓冲）/ Unbind framebuffer (bind default framebuffer)
 */
void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 清除颜色缓冲 / Clear color buffer
 * @param r 红色分量 / Red component
 * @param g 绿色分量 / Green component
 * @param b 蓝色分量 / Blue component
 * @param a Alpha 分量 / Alpha component
 */
void Framebuffer::Clear(float r, float g, float b, float a)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 清除深度缓冲 / Clear depth buffer
 * @param depth 深度值 / Depth value
 */
void Framebuffer::ClearDepth(float depth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 获取帧缓冲宽度 / Get framebuffer width
 * @return 宽度 / Width
 * 
 * 从第一个附件获取宽度。
 * Gets width from the first attachment.
 */
unsigned int Framebuffer::GetWidth() const
{
    if (!m_ColorAttachments.empty())
        return m_ColorAttachments[0].texture->GetWidth();
    if (m_DepthAttachment)
        return m_DepthAttachment->GetWidth();
    if (m_DepthStencilAttachment)
        return m_DepthStencilAttachment->GetWidth();
    return 0;
}

/**
 * @brief 获取帧缓冲高度 / Get framebuffer height
 * @return 高度 / Height
 * 
 * 从第一个附件获取高度。
 * Gets height from the first attachment.
 */
unsigned int Framebuffer::GetHeight() const
{
    if (!m_ColorAttachments.empty())
        return m_ColorAttachments[0].texture->GetHeight();
    if (m_DepthAttachment)
        return m_DepthAttachment->GetHeight();
    if (m_DepthStencilAttachment)
        return m_DepthStencilAttachment->GetHeight();
    return 0;
}
