#include "rendergraph/Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(const std::string& name)
    : RenderResource(name, ResourceType::Framebuffer), FBO(0)
{
}

Framebuffer::~Framebuffer()
{
    Destroy();
}

void Framebuffer::Create()
{
    if (FBO != 0)
        return;

    glGenFramebuffers(1, &FBO);
}

void Framebuffer::Destroy()
{
    if (FBO != 0)
    {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
}

void Framebuffer::AddColorAttachment(std::shared_ptr<Texture> texture, unsigned int index)
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

void Framebuffer::SetDepthAttachment(std::shared_ptr<Texture> texture)
{
    if (!texture)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture->Create();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID, 0);
    m_DepthAttachment = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::SetDepthStencilAttachment(std::shared_ptr<Texture> texture)
{
    if (!texture)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    texture->Create();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID, 0);
    m_DepthStencilAttachment = texture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Clear(float r, float g, float b, float a)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::ClearDepth(float depth)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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
