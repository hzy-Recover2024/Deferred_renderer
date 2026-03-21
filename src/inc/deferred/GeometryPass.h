#ifndef GEOMETRY_PASS_H
#define GEOMETRY_PASS_H

#include "rendergraph/RenderPass.h"
#include "GBuffer.h"
#include "shader_shaderc.h"
#include <glm/glm.hpp>
#include <memory>

class GeometryPass : public RenderPass
{
public:
    GeometryPass(const std::string& name, std::shared_ptr<GBuffer> gbuffer);
    ~GeometryPass() = default;

    void Setup() override;
    void Execute() override;

    void SetProjection(const glm::mat4& projection) { m_Projection = projection; }
    void SetView(const glm::mat4& view) { m_View = view; }
    void SetShader(Shader* shader) { m_Shader = shader; }

    std::shared_ptr<GBuffer> GetGBuffer() const { return m_GBuffer; }

private:
    std::shared_ptr<GBuffer> m_GBuffer;
    Shader* m_Shader;
    glm::mat4 m_Projection;
    glm::mat4 m_View;
};

#endif
