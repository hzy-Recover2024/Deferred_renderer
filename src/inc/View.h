/**
 * @file View.h
 * @brief 视图类定义 / View class definition
 * 
 * 封装了视口、渲染目标和相机，代表一个独立的渲染视图。
 * Encapsulates viewport, render target and camera, 
 * representing an independent render view.
 */

#ifndef VIEW_H
#define VIEW_H

#include "Camera.h"
#include "Framebuffer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

/**
 * @brief 视口结构体 / Viewport structure
 * 
 * 定义视口的位置和尺寸。
 * Defines the position and size of the viewport.
 */
struct Viewport
{
    int X;          ///< 视口左下角 X 坐标 / Viewport lower-left X coordinate
    int Y;          ///< 视口左下角 Y 坐标 / Viewport lower-left Y coordinate
    int Width;      ///< 视口宽度 / Viewport width
    int Height;     ///< 视口高度 / Viewport height

    Viewport() : X(0), Y(0), Width(800), Height(600) {}

    Viewport(int x, int y, int width, int height) 
        : X(x), Y(y), Width(width), Height(height) {}

    float GetAspectRatio() const 
    { 
        return static_cast<float>(Width) / static_cast<float>(Height); 
    }
};

/**
 * @brief 视图类 / View class
 * 
 * 封装了视口、渲染目标和相机，代表一个独立的渲染视图。
 * Encapsulates viewport, render target and camera, 
 * representing an independent render view.
 * 
 * 职责 / Responsibilities:
 * 1. 管理视口参数（位置、尺寸）/ Manage viewport parameters (position, size)
 * 2. 管理渲染目标（FBO 或默认帧缓冲）/ Manage render target (FBO or default framebuffer)
 * 3. 管理相机 / Manage camera
 * 4. 在渲染前设置 OpenGL 状态 / Set OpenGL state before rendering
 * 
 * 使用示例 / Usage example:
 * @code
 * View view;
 * view.SetViewport(0, 0, 1920, 1080);
 * view.SetCamera(std::make_shared<Camera>());
 * 
 * // 渲染前 / Before rendering
 * view.Bind();
 * view.Clear(0.1f, 0.1f, 0.1f, 1.0f);
 * 
 * // 渲染... / Rendering...
 * 
 * view.Unbind();
 * @endcode
 */
class View
{
public:
    /**
     * @brief 默认构造函数 / Default constructor
     */
    View();

    /**
     * @brief 构造函数 / Constructor
     * @param viewport 视口参数 / Viewport parameters
     */
    explicit View(const Viewport& viewport);

    /**
     * @brief 析构函数 / Destructor
     */
    ~View() = default;

    // ==================== 视口操作 / Viewport Operations ====================

    /**
     * @brief 设置视口 / Set viewport
     * @param x 左下角 X 坐标 / Lower-left X coordinate
     * @param y 左下角 Y 坐标 / Lower-left Y coordinate
     * @param width 宽度 / Width
     * @param height 高度 / Height
     */
    void SetViewport(int x, int y, int width, int height);

    /**
     * @brief 设置视口 / Set viewport
     * @param viewport 视口参数 / Viewport parameters
     */
    void SetViewport(const Viewport& viewport);

    /**
     * @brief 获取视口 / Get viewport
     * @return const Viewport& 视口引用 / Viewport reference
     */
    const Viewport& GetViewport() const { return m_Viewport; }

    /**
     * @brief 获取视口宽度 / Get viewport width
     */
    int GetWidth() const { return m_Viewport.Width; }

    /**
     * @brief 获取视口高度 / Get viewport height
     */
    int GetHeight() const { return m_Viewport.Height; }

    /**
     * @brief 获取宽高比 / Get aspect ratio
     */
    float GetAspectRatio() const { return m_Viewport.GetAspectRatio(); }

    // ==================== 渲染目标操作 / Render Target Operations ====================

    /**
     * @brief 设置渲染目标 / Set render target
     * 
     * 设置渲染输出目标。传入 nullptr 表示渲染到默认帧缓冲（屏幕）。
     * Sets render output target. nullptr means render to default framebuffer (screen).
     * 
     * @param framebuffer 帧缓冲智能指针 / Framebuffer smart pointer
     */
    void SetRenderTarget(RefPtr<Framebuffer> framebuffer);

    /**
     * @brief 获取渲染目标 / Get render target
     * @return RefPtr<Framebuffer> 帧缓冲智能指针 / Framebuffer smart pointer
     */
    RefPtr<Framebuffer> GetRenderTarget() const { return m_RenderTarget; }

    /**
     * @brief 是否渲染到屏幕 / Whether rendering to screen
     * @return true 渲染到默认帧缓冲 / Rendering to default framebuffer
     * @return false 渲染到自定义帧缓冲 / Rendering to custom framebuffer
     */
    bool IsRenderingToScreen() const { return m_RenderTarget == nullptr; }

    // ==================== 相机操作 / Camera Operations ====================

    /**
     * @brief 设置相机 / Set camera
     * @param camera 相机智能指针 / Camera smart pointer
     */
    void SetCamera(std::shared_ptr<Camera> camera);

    /**
     * @brief 获取相机 / Get camera
     * @return std::shared_ptr<Camera> 相机智能指针 / Camera smart pointer
     */
    std::shared_ptr<Camera> GetCamera() const { return m_Camera; }

    /**
     * @brief 获取视图矩阵 / Get view matrix
     * @return glm::mat4 视图矩阵 / View matrix
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * @brief 获取投影矩阵 / Get projection matrix
     * @return glm::mat4 投影矩阵 / Projection matrix
     */
    glm::mat4 GetProjectionMatrix() const;

    /**
     * @brief 获取相机位置 / Get camera position
     * @return glm::vec3 相机位置 / Camera position
     */
    glm::vec3 GetCameraPosition() const;

    // ==================== 渲染操作 / Render Operations ====================

    /**
     * @brief 绑定视图 / Bind view
     * 
     * 设置视口和绑定渲染目标。在渲染前调用。
     * Sets viewport and binds render target. Call before rendering.
     */
    void Bind() const;

    /**
     * @brief 解绑视图 / Unbind view
     * 
     * 解绑渲染目标。在渲染后调用。
     * Unbinds render target. Call after rendering.
     */
    void Unbind() const;

    /**
     * @brief 清除视图 / Clear view
     * 
     * 清除颜色缓冲和深度缓冲。
     * Clears color and depth buffers.
     * 
     * @param r 红色分量 / Red component
     * @param g 绿色分量 / Green component
     * @param b 蓝色分量 / Blue component
     * @param a Alpha 分量 / Alpha component
     */
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

private:
    Viewport m_Viewport;                        ///< 视口 / Viewport
    RefPtr<Framebuffer> m_RenderTarget;         ///< 渲染目标 / Render target
    std::shared_ptr<Camera> m_Camera;           ///< 相机 / Camera
};

#endif
