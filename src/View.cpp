/**
 * @file View.cpp
 * @brief 视图类实现
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

static_assert(sizeof(GPUCameraData) == 416, "GPUCameraData size must be 416 bytes for std140 layout");

}

View::View()
    : m_Viewport()
    , m_RenderTarget(nullptr)
    , m_Camera(std::make_shared<Camera>())
    , m_CameraUBO(nullptr)
    , m_Dirty(true)
{
    initUBO();
}

View::View(const Viewport& viewport)
    : m_Viewport(viewport)
    , m_RenderTarget(nullptr)
    , m_Camera(std::make_shared<Camera>())
    , m_CameraUBO(nullptr)
    , m_Dirty(true)
{
    initUBO();
}

View::~View() = default;

void View::initUBO()
{
    m_CameraUBO = MakeRefPtr<UniformBuffer>("CameraUBO", sizeof(GPUCameraData), Camera_binding);
    m_CameraUBO->Create();
}

void View::SetViewport(int x, int y, int width, int height)
{
    m_Viewport.X = x;
    m_Viewport.Y = y;
    m_Viewport.Width = width;
    m_Viewport.Height = height;
    m_Dirty = true;
}

void View::SetViewport(const Viewport& viewport)
{
    m_Viewport = viewport;
    m_Dirty = true;
}

void View::SetRenderTarget(RefPtr<Framebuffer> framebuffer)
{
    m_RenderTarget = framebuffer;
}

void View::SetCamera(std::shared_ptr<Camera> camera)
{
    m_Camera = camera;
    m_Dirty = true;
}

glm::mat4 View::GetViewMatrix() const
{
    return m_Camera ? m_Camera->GetViewMatrix() : glm::mat4(1.0f);
}

glm::mat4 View::GetProjectionMatrix() const
{
    return m_Camera ? m_Camera->GetProjectionMatrix(
        static_cast<float>(m_Viewport.Width),
        static_cast<float>(m_Viewport.Height)) : glm::mat4(1.0f);
}

glm::vec3 View::GetCameraPosition() const
{
    return m_Camera ? m_Camera->Position : glm::vec3(0.0f);
}

void View::UpdateCameraUBO()
{
    if (!m_Dirty || !m_Camera || !m_CameraUBO)
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

    m_CameraUBO->SetData(&gpuData, sizeof(GPUCameraData));
    m_Dirty = false;
}

void View::BindCameraUBO() const
{
    if (m_CameraUBO)
        m_CameraUBO->Bind();
}

void View::Bind() const
{
    glViewport(m_Viewport.X, m_Viewport.Y, m_Viewport.Width, m_Viewport.Height);
    if (m_RenderTarget)
        m_RenderTarget->Bind();
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void View::Unbind() const
{
    if (m_RenderTarget)
        m_RenderTarget->Unbind();
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void View::Clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
