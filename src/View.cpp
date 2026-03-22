/**
 * @file View.cpp
 * @brief 视图类实现 / View class implementation
 */

#include "View.h"
#include "common/uniform_layout.h"
#include <glm/gtc/type_ptr.hpp>

namespace {

struct GPUCameraData
{
    glm::vec4 positionNear;
    glm::vec4 forwardFar;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;
    glm::mat4 invViewMatrix;
    glm::mat4 invProjectionMatrix;
    glm::mat4 invViewProjectionMatrix;
};

}

View::View()
    : m_Viewport()
    , m_RenderTarget(nullptr)
    , m_Camera(std::make_shared<Camera>())
    , m_cameraUBO(0)
{
    initUBO();
}

View::View(const Viewport& viewport)
    : m_Viewport(viewport)
    , m_RenderTarget(nullptr)
    , m_Camera(std::make_shared<Camera>())
    , m_cameraUBO(0)
{
    initUBO();
}

View::~View()
{
    if (m_cameraUBO)
    {
        glDeleteBuffers(1, &m_cameraUBO);
        m_cameraUBO = 0;
    }
}

void View::initUBO()
{
    glGenBuffers(1, &m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUCameraData), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, Camera_binding, m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void View::SetViewport(int x, int y, int width, int height)
{
    m_Viewport.X = x;
    m_Viewport.Y = y;
    m_Viewport.Width = width;
    m_Viewport.Height = height;
}

void View::SetViewport(const Viewport& viewport)
{
    m_Viewport = viewport;
}

void View::SetRenderTarget(RefPtr<Framebuffer> framebuffer)
{
    m_RenderTarget = framebuffer;
}

void View::SetCamera(std::shared_ptr<Camera> camera)
{
    m_Camera = camera;
}

glm::mat4 View::GetViewMatrix() const
{
    if (m_Camera)
    {
        return m_Camera->GetViewMatrix();
    }
    return glm::mat4(1.0f);
}

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

glm::vec3 View::GetCameraPosition() const
{
    if (m_Camera)
    {
        return m_Camera->Position;
    }
    return glm::vec3(0.0f);
}

void View::UpdateCameraUBO()
{
    if (!m_Camera)
        return;

    GPUCameraData gpuData;
    float width = static_cast<float>(m_Viewport.Width);
    float height = static_cast<float>(m_Viewport.Height);

    gpuData.positionNear = glm::vec4(m_Camera->Position, m_Camera->NearPlane);
    gpuData.forwardFar = glm::vec4(m_Camera->Front, m_Camera->FarPlane);
    gpuData.viewMatrix = m_Camera->GetViewMatrix();
    gpuData.projectionMatrix = m_Camera->GetProjectionMatrix(width, height);
    gpuData.viewProjectionMatrix = gpuData.projectionMatrix * gpuData.viewMatrix;
    gpuData.invViewMatrix = glm::inverse(gpuData.viewMatrix);
    gpuData.invProjectionMatrix = glm::inverse(gpuData.projectionMatrix);
    gpuData.invViewProjectionMatrix = glm::inverse(gpuData.viewProjectionMatrix);

    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUCameraData), &gpuData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void View::BindCameraUBO() const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, Camera_binding, m_cameraUBO);
}

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

void View::Clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
