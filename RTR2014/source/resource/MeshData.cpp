#include "resource/MeshData.h"

MeshData::MeshData(std::vector<float> vertices, std::vector<unsigned int> indices,
             std::vector<float> normals, std::vector<float> uvs, PrimitiveType type)
    : m_vertices(std::move(vertices)), m_indices(std::move(indices)),
      m_normals(std::move(normals)), m_uvs(std::move(uvs)), m_type(type)
{
    return;
}

MeshData::MeshData() : m_type(PrimitiveType::Invalid) { return; }
