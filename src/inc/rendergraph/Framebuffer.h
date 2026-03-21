#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "RenderResource.h"
#include "Texture.h"
#include <glad/glad.h>
#include <vector>
#include <memory>

class Framebuffer : public RenderResource
{
public:
    struct ColorAttachment {
        std::shared_ptr<Texture> texture;
        unsigned int Index;
    };

    unsigned int FBO;

    Framebuffer(const std::string& name);
    ~Framebuffer();

    void Create() override;
    void Destroy() override;
    bool IsValid() const override { return FBO != 0; }

    void AddColorAttachment(std::shared_ptr<Texture> texture, unsigned int index = 0);
    void SetDepthAttachment(std::shared_ptr<Texture> texture);
    void SetDepthStencilAttachment(std::shared_ptr<Texture> texture);

    void Bind() const;
    void Unbind() const;

    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void ClearDepth(float depth = 1.0f);

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    const std::vector<ColorAttachment>& GetColorAttachments() const { return m_ColorAttachments; }
    std::shared_ptr<Texture> GetDepthAttachment() const { return m_DepthAttachment; }
    std::shared_ptr<Texture> GetDepthStencilAttachment() const { return m_DepthStencilAttachment; }

private:
    std::vector<ColorAttachment> m_ColorAttachments;
    std::shared_ptr<Texture> m_DepthAttachment;
    std::shared_ptr<Texture> m_DepthStencilAttachment;
};

#endif
