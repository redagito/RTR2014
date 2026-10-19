#include "resource/MeshData.h"

MeshData::MeshData(std::vector<float> vertices, std::vector<unsigned int> indices,
             std::vector<float> normals, std::vector<float> uvs, PrimitiveType type)
    : m_vertices(vertices), m_indices(indices), m_normals(normals), m_uvs(uvs), m_type(type)
{
    return;
}

MeshData::MeshData() : m_type(PrimitiveType::Invalid) { return; }
