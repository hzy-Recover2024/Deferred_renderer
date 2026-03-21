#ifndef GBUFFER_H
#define GBUFFER_H

#include "rendergraph/Texture.h"
#include "rendergraph/Framebuffer.h"
#include <glad/glad.h>
#include <memory>

class GBuffer
{
public:
    enum GBufferTexture {
        POSITION = 0,
        NORMAL,
        ALBEDO_SPEC,
        DEPTH,
        COUNT
    };

    GBuffer();
    ~GBuffer() = default;

    bool Init(unsigned int width, unsigned int height);
    void Resize(unsigned int width, unsigned int height);

    void BindForWriting() const;
    void BindForReading() const;
    void Unbind() const;

    void SetReadBuffer(GBufferTexture textureType);
    void BindTexture(GBufferTexture textureType, unsigned int slot) const;

    std::shared_ptr<Texture> GetTexture(GBufferTexture type) const;
    std::shared_ptr<Framebuffer> GetFramebuffer() const { return m_Framebuffer; }

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

private:
    std::shared_ptr<Framebuffer> m_Framebuffer;
    std::shared_ptr<Texture> m_Textures[COUNT];
    unsigned int m_Width;
    unsigned int m_Height;
};

#endif
