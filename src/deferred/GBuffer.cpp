#include "deferred/GBuffer.h"
#include <iostream>

GBuffer::GBuffer()
    : m_Width(0), m_Height(0)
{
}

bool GBuffer::Init(unsigned int width, unsigned int height)
{
    m_Width = width;
    m_Height = height;

    m_Framebuffer = std::make_shared<Framebuffer>("GBuffer");
    m_Framebuffer->Create();

    Texture::Desc positionDesc;
    positionDesc.Width = width;
    positionDesc.Height = height;
    positionDesc.InternalFormat = GL_RGBA16F;
    positionDesc.Format = GL_RGBA;
    positionDesc.Type = GL_FLOAT;
    positionDesc.FilterMin = GL_NEAREST;
    positionDesc.FilterMag = GL_NEAREST;
    m_Textures[POSITION] = std::make_shared<Texture>("GPosition", positionDesc);

    Texture::Desc normalDesc;
    normalDesc.Width = width;
    normalDesc.Height = height;
    normalDesc.InternalFormat = GL_RGBA16F;
    normalDesc.Format = GL_RGBA;
    normalDesc.Type = GL_FLOAT;
    normalDesc.FilterMin = GL_NEAREST;
    normalDesc.FilterMag = GL_NEAREST;
    m_Textures[NORMAL] = std::make_shared<Texture>("GNormal", normalDesc);

    Texture::Desc albedoDesc;
    albedoDesc.Width = width;
    albedoDesc.Height = height;
    albedoDesc.InternalFormat = GL_RGBA;
    albedoDesc.Format = GL_RGBA;
    albedoDesc.Type = GL_UNSIGNED_BYTE;
    albedoDesc.FilterMin = GL_NEAREST;
    albedoDesc.FilterMag = GL_NEAREST;
    m_Textures[ALBEDO_SPEC] = std::make_shared<Texture>("GAlbedoSpec", albedoDesc);

    Texture::Desc depthDesc;
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.InternalFormat = GL_DEPTH24_STENCIL8;
    depthDesc.Format = GL_DEPTH_STENCIL;
    depthDesc.Type = GL_UNSIGNED_INT_24_8;
    depthDesc.FilterMin = GL_NEAREST;
    depthDesc.FilterMag = GL_NEAREST;
    m_Textures[DEPTH] = std::make_shared<Texture>("GDepth", depthDesc);

    m_Framebuffer->AddColorAttachment(m_Textures[POSITION], 0);
    m_Framebuffer->AddColorAttachment(m_Textures[NORMAL], 1);
    m_Framebuffer->AddColorAttachment(m_Textures[ALBEDO_SPEC], 2);
    m_Framebuffer->SetDepthStencilAttachment(m_Textures[DEPTH]);

    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->FBO);
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "GBuffer framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void GBuffer::Resize(unsigned int width, unsigned int height)
{
    if (m_Width == width && m_Height == height)
        return;

    m_Width = width;
    m_Height = height;

    m_Textures[POSITION]->Resize(width, height);
    m_Textures[NORMAL]->Resize(width, height);
    m_Textures[ALBEDO_SPEC]->Resize(width, height);
    m_Textures[DEPTH]->Resize(width, height);
}

void GBuffer::BindForWriting() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->FBO);
}

void GBuffer::BindForReading() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::SetReadBuffer(GBufferTexture textureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}

void GBuffer::BindTexture(GBufferTexture textureType, unsigned int slot) const
{
    if (textureType < COUNT && m_Textures[textureType])
    {
        m_Textures[textureType]->Bind(slot);
    }
}

std::shared_ptr<Texture> GBuffer::GetTexture(GBufferTexture type) const
{
    if (type < COUNT)
        return m_Textures[type];
    return nullptr;
}
