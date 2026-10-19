#include "graphics/renderer/Renderer.h"

#include <cassert>
#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "graphics/Mesh.h"


Renderer::Renderer()
{
	return;
}

Renderer::~Renderer()
{
	return;
}

void Renderer::draw(Mesh* mesh)
{
    mesh->getVertexArray()->setActive();

    // Set primitive draw mode
    GLenum mode = Mesh::toGLPrimitive(mesh->getPrimitiveType());
    unsigned int primitiveSize = Mesh::getPrimitiveSize(mesh->getPrimitiveType());

    // Decide on draw method based on the stored data
    if (mesh->hasIndexBuffer())
    {
        // Indexed draw, faster
        mesh->getIndexBuffer()->setActive();
        glDrawElements(mode, mesh->getIndexBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
        mesh->getIndexBuffer()->setInactive();
    }
    else
    {
        // Slowest draw method
        glDrawArrays(mode, 0, mesh->getVertexBuffer()->getSize() / primitiveSize);
    }
    mesh->getVertexArray()->setInactive();
}
