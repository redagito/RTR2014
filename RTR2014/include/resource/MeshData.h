#pragma once

#include <vector>

#include "resource/OrientedBoundingBox.h"
#include "resource/ResourceConfig.h"

/**
 * \brief Mesh data.
 */
struct MeshData
{
    MeshData();
    MeshData(std::vector<float> vertices, std::vector<unsigned int> indices,
          std::vector<float> normals, std::vector<float> uvs, PrimitiveType type);
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<float> m_normals;
    std::vector<float> m_uvs;
    PrimitiveType m_type;
    OrientedBoundingBox m_bounds; /**< Extent of the mesh in mesh space. */
};
