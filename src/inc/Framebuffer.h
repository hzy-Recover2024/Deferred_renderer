/**
 * @file Framebuffer.h
 * @brief 帧缓冲资源类定义 / Framebuffer resource class definition
 * 
 * 封装 OpenGL 帧缓冲对象（FBO），支持多渲染目标（MRT）。
 * Encapsulates OpenGL framebuffer object (FBO), supporting multiple render targets (MRT).
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "RenderResource.h"
#include "Texture.h"
#include <glad/glad.h>
#include <vector>

/**
 * @brief 帧缓冲资源类 / Framebuffer resource class
 * 
 * 继承自 RenderResource，管理 OpenGL 帧缓冲对象的生命周期。
 * Inherits from RenderResource, manages lifecycle of OpenGL framebuffer object.
 * 
 * 支持功能 / Supported features:
 * - 多个颜色附件（MRT）/ Multiple color attachments (MRT)
 * - 深度附件 / Depth attachment
 * - 深度模板附件 / Depth-stencil attachment
 * - 清除缓冲 / Buffer clearing
 * 
 * 使用示例 / Usage example:
 * @code
 * auto fb = MakeRefPtr<Framebuffer>("GBuffer");
 * fb->AddColorAttachment(positionTex, 0);
 * fb->AddColorAttachment(normalTex, 1);
 * fb->SetDepthAttachment(depthTex);
 * fb->Create();
 * 
 * fb->Bind();
 * // 渲染操作... / Rendering operations...
 * fb->Unbind();
 * @endcode
 */
class Framebuffer : public RenderResource
{
public:
    /**
     * @brief 颜色附件结构体 / Color attachment structure
     */
    struct ColorAttachment {
        RefPtr<Texture> texture;  ///< 颜色纹理 / Color texture
        unsigned int Index;       ///< 附件索引 / Attachment index
    };

    unsigned int FBO;  ///< OpenGL 帧缓冲 ID / OpenGL framebuffer ID

    /**
     * @brief 构造函数 / Constructor
     * @param name 帧缓冲名称 / Framebuffer name
     */
    Framebuffer(const std::string& name);

    /**
     * @brief 析构函数，自动销毁帧缓冲 / Destructor, automatically destroys framebuffer
     */
    ~Framebuffer();

    /**
     * @brief 创建 GPU 帧缓冲资源 / Create GPU framebuffer resource
     * 
     * 调用 glGenFramebuffers 创建帧缓冲对象。
     * Calls glGenFramebuffers to create framebuffer object.
     */
    void Create() override;

    /**
     * @brief 销毁 GPU 帧缓冲资源 / Destroy GPU framebuffer resource
     * 
     * 调用 glDeleteFramebuffers 删除帧缓冲对象。
     * Calls glDeleteFramebuffers to delete framebuffer object.
     */
    void Destroy() override;

    /**
     * @brief 检查帧缓冲是否有效 / Check if framebuffer is valid
     * @return true FBO 非 0 / FBO is non-zero
     * @return false FBO 为 0 / FBO is zero
     */
    bool IsValid() const override { return FBO != 0; }

    /**
     * @brief 添加颜色附件 / Add color attachment
     * @param texture 颜色纹理 / Color texture
     * @param index 附件索引，默认为 0 / Attachment index, default is 0
     */
    void AddColorAttachment(RefPtr<Texture> texture, unsigned int index = 0);

    /**
     * @brief 设置深度附件 / Set depth attachment
     * @param texture 深度纹理 / Depth texture
     */
    void SetDepthAttachment(RefPtr<Texture> texture);

    /**
     * @brief 设置深度模板附件 / Set depth-stencil attachment
     * @param texture 深度模板纹理 / Depth-stencil texture
     */
    void SetDepthStencilAttachment(RefPtr<Texture> texture);

    /**
     * @brief 绑定帧缓冲 / Bind framebuffer
     */
    void Bind() const;

    /**
     * @brief 解绑帧缓冲（绑定默认帧缓冲）/ Unbind framebuffer (bind default framebuffer)
     */
    void Unbind() const;

    /**
     * @brief 清除颜色缓冲 / Clear color buffer
     * @param r 红色分量 / Red component
     * @param g 绿色分量 / Green component
     * @param b 蓝色分量 / Blue component
     * @param a Alpha 分量 / Alpha component
     */
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    /**
     * @brief 清除深度缓冲 / Clear depth buffer
     * @param depth 深度值 / Depth value
     */
    void ClearDepth(float depth = 1.0f);

    /**
     * @brief 获取帧缓冲宽度 / Get framebuffer width
     * @return 宽度 / Width
     */
    unsigned int GetWidth() const;

    /**
     * @brief 获取帧缓冲高度 / Get framebuffer height
     * @return 高度 / Height
     */
    unsigned int GetHeight() const;

    /**
     * @brief 获取所有颜色附件 / Get all color attachments
     * @return 颜色附件列表 / List of color attachments
     */
    const std::vector<ColorAttachment>& GetColorAttachments() const { return m_ColorAttachments; }

    /**
     * @brief 获取深度附件 / Get depth attachment
     * @return 深度纹理 / Depth texture
     */
    RefPtr<Texture> GetDepthAttachment() const { return m_DepthAttachment; }

    /**
     * @brief 获取深度模板附件 / Get depth-stencil attachment
     * @return 深度模板纹理 / Depth-stencil texture
     */
    RefPtr<Texture> GetDepthStencilAttachment() const { return m_DepthStencilAttachment; }

private:
    std::vector<ColorAttachment> m_ColorAttachments;  ///< 颜色附件列表 / Color attachment list
    RefPtr<Texture> m_DepthAttachment;                ///< 深度附件 / Depth attachment
    RefPtr<Texture> m_DepthStencilAttachment;         ///< 深度模板附件 / Depth-stencil attachment
};

#endif
