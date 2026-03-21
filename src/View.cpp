/**
 * @file View.cpp
 * @brief 视图类实现 / View class implementation
 */

#include "View.h"

/**
 * @brief 默认构造函数实现 / Default constructor implementation
 */
View::View()
    : m_Viewport(), m_RenderTarget(nullptr), m_Camera(std::make_shared<Camera>())
{
}

/**
 * @brief 构造函数实现 / Constructor implementation
 */
View::View(const Viewport& viewport)
    : m_Viewport(viewport), m_RenderTarget(nullptr), m_Camera(std::make_shared<Camera>())
{
}

// ==================== 视口操作 / Viewport Operations ====================

/**
 * @brief 设置视口 / Set viewport
 */
void View::SetViewport(int x, int y, int width, int height)
{
    m_Viewport.X = x;
    m_Viewport.Y = y;
    m_Viewport.Width = width;
    m_Viewport.Height = height;
}

/**
 * @brief 设置视口 / Set viewport
 */
void View::SetViewport(const Viewport& viewport)
{
    m_Viewport = viewport;
}

// ==================== 渲染目标操作 / Render Target Operations ====================

/**
 * @brief 设置渲染目标 / Set render target
 */
void View::SetRenderTarget(RefPtr<Framebuffer> framebuffer)
{
    m_RenderTarget = framebuffer;
}

// ==================== 相机操作 / Camera Operations ====================

/**
 * @brief 设置相机 / Set camera
 */
void View::SetCamera(std::shared_ptr<Camera> camera)
{
    m_Camera = camera;
}

/**
 * @brief 获取视图矩阵 / Get view matrix
 */
glm::mat4 View::GetViewMatrix() const
{
    if (m_Camera)
    {
        return m_Camera->GetViewMatrix();
    }
    return glm::mat4(1.0f);
}

/**
 * @brief 获取投影矩阵 / Get projection matrix
 */
glm::mat4 View::GetProjectionMatrix() const
{
    if (m_Camera)
    {
        return m_Camera->GetProjectionMatrix(
            static_cast<float>(m_Viewport.Width),
            static_cast<float>(m_Viewport.Height)
        );
    }
    return glm::mat4(1.0f);
}

/**
 * @brief 获取相机位置 / Get camera position
 */
glm::vec3 View::GetCameraPosition() const
{
    if (m_Camera)
    {
        return m_Camera->Position;
    }
    return glm::vec3(0.0f);
}

// ==================== 渲染操作 / Render Operations ====================

/**
 * @brief 绑定视图 / Bind view
 */
void View::Bind() const
{
    glViewport(m_Viewport.X, m_Viewport.Y, m_Viewport.Width, m_Viewport.Height);

    if (m_RenderTarget)
    {
        m_RenderTarget->Bind();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

/**
 * @brief 解绑视图 / Unbind view
 */
void View::Unbind() const
{
    if (m_RenderTarget)
    {
        m_RenderTarget->Unbind();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

/**
 * @brief 清除视图 / Clear view
 */
void View::Clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
