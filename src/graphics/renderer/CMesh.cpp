#include "CMesh.h"
#include "debug/Log.h"

CMesh::CMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
             const std::vector<float>& normals, const std::vector<float>& uvs, EPrimitiveType type)
    : m_vertices(nullptr), m_normals(nullptr), m_uvs(nullptr), m_type(EPrimitiveType::Invalid)
{
    init(vertices, indices, normals, uvs, type);
}

CMesh::~CMesh() { return; }

bool CMesh::init(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
                 const std::vector<float>& normals, const std::vector<float>& uvs,
                 EPrimitiveType type)
{
    if (vertices.empty() || type == EPrimitiveType::Invalid)
    {
        return false;
    }
    // Set vertex data
    m_vertices.reset(new CVertexBuffer(vertices));

    // Set indices
    m_indices.reset(new CIndexBuffer(indices));

    // Set normals
    m_normals.reset(new CVertexBuffer(normals));

    // Set uvs
    m_uvs.reset(new CVertexBuffer(uvs));

    // Set primitive type
    m_type = type;
    return true;
}

const std::unique_ptr<CVertexBuffer>& CMesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<CIndexBuffer>& CMesh::getIndexBuffer() const { return m_indices; }

const std::unique_ptr<CVertexBuffer>& CMesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<CVertexBuffer>& CMesh::getUVBuffer() const { return m_uvs; }
