#include "ARenderer.h"

#include <cassert>
#include <string>

#include "core/RendererCoreConfig.h"
#include "graphics/resource/CMesh.h"

#include "debug/Log.h"

ARenderer::ARenderer()
{
	return;
}

ARenderer::~ARenderer()
{
	return;
}

void ARenderer::draw(CMesh* mesh)
{
    mesh->getVertexArray()->setActive();

    // Set primitive draw mode
    GLenum mode = CMesh::toGLPrimitive(mesh->getPrimitiveType());
    unsigned int primitiveSize = CMesh::getPrimitiveSize(mesh->getPrimitiveType());

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