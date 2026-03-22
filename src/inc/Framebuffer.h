/**
 * @file Framebuffer.h
 * @brief 帧缓冲资源类
 * 
 * 封装 OpenGL 帧缓冲对象（FBO），支持多渲染目标（MRT）。
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "RenderResource.h"
#include "Texture.h"
#include <glad/glad.h>
#include <vector>

/**
 * @brief 帧缓冲资源类
 * 
 * 管理帧缓冲对象的生命周期，支持：
 * - 多个颜色附件（MRT）
 * - 深度附件
 * - 深度模板附件
 */
class Framebuffer : public RenderResource
{
public:
    struct ColorAttachment {
        RefPtr<Texture> texture;
        unsigned int Index;
    };

    unsigned int FBO;

    Framebuffer(const std::string& name);
    ~Framebuffer();

    void Create() override;
    void Destroy() override;
    bool IsValid() const override { return FBO != 0; }

    void AddColorAttachment(RefPtr<Texture> texture, unsigned int index = 0);
    void SetDepthAttachment(RefPtr<Texture> texture);
    void SetDepthStencilAttachment(RefPtr<Texture> texture);

    void Bind() const;
    void Unbind() const;

    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void ClearDepth(float depth = 1.0f);

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

    const std::vector<ColorAttachment>& GetColorAttachments() const { return m_ColorAttachments; }
    RefPtr<Texture> GetDepthAttachment() const { return m_DepthAttachment; }
    RefPtr<Texture> GetDepthStencilAttachment() const { return m_DepthStencilAttachment; }

private:
    std::vector<ColorAttachment> m_ColorAttachments;
    RefPtr<Texture> m_DepthAttachment;
    RefPtr<Texture> m_DepthStencilAttachment;
};

#endif
