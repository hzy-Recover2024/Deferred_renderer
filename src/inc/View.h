/**
 * @file View.h
 * @brief 视图类，封装视口、渲染目标和相机
 */

#ifndef VIEW_H
#define VIEW_H

#include "Camera.h"
#include "Framebuffer.h"
#include "UniformBuffer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

struct Viewport
{
    int X;
    int Y;
    int Width;
    int Height;

    Viewport() : X(0), Y(0), Width(800), Height(600) {}
    Viewport(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height) {}

    float GetAspectRatio() const { return static_cast<float>(Width) / static_cast<float>(Height); }
};

class View
{
public:
    View();
    explicit View(const Viewport& viewport);
    ~View();

    View(const View&) = delete;
    View& operator=(const View&) = delete;

    void SetViewport(int x, int y, int width, int height);
    void SetViewport(const Viewport& viewport);
    const Viewport& GetViewport() const { return m_Viewport; }
    int GetWidth() const { return m_Viewport.Width; }
    int GetHeight() const { return m_Viewport.Height; }
    float GetAspectRatio() const { return m_Viewport.GetAspectRatio(); }

    void SetRenderTarget(RefPtr<Framebuffer> framebuffer);
    RefPtr<Framebuffer> GetRenderTarget() const { return m_RenderTarget; }
    bool IsRenderingToScreen() const { return m_RenderTarget == nullptr; }

    void SetCamera(std::shared_ptr<Camera> camera);
    std::shared_ptr<Camera> GetCamera() const { return m_Camera; }
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::vec3 GetCameraPosition() const;

    void MarkDirty() { m_Dirty = true; }
    void UpdateCameraUBO();
    void BindCameraUBO() const;

    void Bind() const;
    void Unbind() const;
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

private:
    void initUBO();

private:
    Viewport m_Viewport;
    RefPtr<Framebuffer> m_RenderTarget;
    std::shared_ptr<Camera> m_Camera;
    RefPtr<UniformBuffer> m_CameraUBO;
    bool m_Dirty = true;
};

#endif
