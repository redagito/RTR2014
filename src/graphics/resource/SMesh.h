#pragma once

#include <vector>

#include "graphics/ResourceConfig.h"

/**
 * \brief Mesh data.
 */
struct SMesh
{
    SMesh();
    SMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
          std::vector<float> normals, std::vector<float> uvs, EPrimitiveType type);
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<float> m_normals;
    std::vector<float> m_uvs;
    EPrimitiveType m_type;
};
