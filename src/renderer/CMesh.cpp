#include "CMesh.h"
#include "debug/Log.h"

CMesh::CMesh(std::unique_ptr<CVertexBuffer> vertices, std::unique_ptr<CVertexBuffer> normals,
             std::unique_ptr<CVertexBuffer> uvs)
    : m_vertices(std::move(vertices)), m_normals(std::move(normals)), m_uvs(std::move(uvs))
{
}

CMesh::~CMesh() { return; }

const std::unique_ptr<CVertexBuffer>& CMesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<CVertexBuffer>& CMesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<CVertexBuffer>& CMesh::getUVBuffer() const { return m_uvs; }
