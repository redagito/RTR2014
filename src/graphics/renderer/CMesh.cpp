#include "CMesh.h"
#include "debug/Log.h"

CMesh::CMesh(const std::vector<float>& vertices, const std::vector<float>& normals,
             const std::vector<float>& uvs)
    : m_vertices(new CVertexBuffer(vertices)),
      m_normals(new CVertexBuffer(normals)),
      m_uvs(new CVertexBuffer(uvs))
{
}

CMesh::~CMesh() { return; }

const std::unique_ptr<CVertexBuffer>& CMesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<CVertexBuffer>& CMesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<CVertexBuffer>& CMesh::getUVBuffer() const { return m_uvs; }
