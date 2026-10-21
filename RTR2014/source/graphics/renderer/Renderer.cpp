#include "graphics/renderer/Renderer.h"

#include <cassert>
#include <string>

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/core/RendererCoreConfig.h"

Renderer::Renderer() { return; }

Renderer::~Renderer() { return; }

void Renderer::draw(const Mesh* mesh)
{
    mesh->getVertexArray()->setActive();

    // Set primitive draw mode
    GLenum mode = Mesh::toGLPrimitive(mesh->getPrimitiveType());
    unsigned int primitiveSize = Mesh::getPrimitiveSize(mesh->getPrimitiveType());
    if (primitiveSize == 0)
    {
        spdlog::error("A mesh with an unknown primitive type cannot be drawn.");
        mesh->getVertexArray()->setInactive();
        return;
    }

    // Decide on draw method based on the stored data
    if (mesh->hasIndexBuffer())
    {
        // Indexed draw, faster
        glDrawElements(mode, mesh->getIndexBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        // Slowest draw method
        glDrawArrays(mode, 0, mesh->getVertexBuffer()->getSize() / primitiveSize);
    }
    mesh->getVertexArray()->setInactive();
}

void Renderer::bindTexture(ShaderProgram* shader, const std::string& uniformName, GLint unit,
                           const Texture* texture, const Texture* fallback)
{
    if (texture != nullptr)
    {
        texture->setActive(unit);
    }
    else
    {
        fallback->setActive(unit);
    }
    shader->setUniform(uniformName, unit);
}
