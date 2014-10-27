#include "CMesh.h"
#include "debug/Log.h"

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "graphics/renderer/debug/RendererDebug.h"

#include <cassert>

CMesh::CMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
             const std::vector<float>& normals, const std::vector<float>& uvs, EPrimitiveType type)
    : m_vertices(nullptr),
      m_indices(nullptr),
      m_normals(nullptr),
      m_uvs(nullptr),
      m_vao(nullptr),
      m_type(EPrimitiveType::Invalid)
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

    if (!indices.empty())
    {
        // Set indices
        m_indices.reset(new CIndexBuffer(indices));
    }

    if (!normals.empty())
    {
        // Set normals
        m_normals.reset(new CVertexBuffer(normals));
    }

    if (!uvs.empty())
    {
        // Set uvs
        m_uvs.reset(new CVertexBuffer(uvs));
    }

    // Create new vertex array object to store buffer state
    m_vao.reset(new CVertexArrayObject);

    // Set primitive type
    m_type = type;

    // Initialize state
    m_vao->setActive();

    // Sanity check
    if (!m_vertices->isValid())
    {
        return false;
    }

    // Set vertex data attributes
    // TODO The location should be in some kind of shader interface definition
    // TODO The data data size (second param) should be deduced by the data type of the provided
    // vertex data
    //  basically we would need a std::vector<glm::vec3> instead of a std::vector<float> to deduce
    //  the size
    // TODO Consider packing all vertex data into a single buffer for better performance
    m_vertices->setActive();
    glVertexAttribPointer(vertexDataShaderLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexDataShaderLocation);

    // Set normal data attributes
    if (m_normals != nullptr)
    {
        m_normals->setActive();
        glVertexAttribPointer(normalDataShaderLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
        m_normals->setInactive();
        glEnableVertexAttribArray(normalDataShaderLocation);
    }

    // Set uv data attributes
    if (m_uvs != nullptr)
    {
        m_uvs->setActive();
        glVertexAttribPointer(uvDataShaderLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(uvDataShaderLocation);
    }

    // Disable vao
    m_vao->setInactive();

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    return true;
}

bool CMesh::hasIndexBuffer() const { return m_indices != nullptr; }

const std::unique_ptr<CVertexBuffer>& CMesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<CIndexBuffer>& CMesh::getIndexBuffer() const { return m_indices; }

const std::unique_ptr<CVertexBuffer>& CMesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<CVertexBuffer>& CMesh::getUVBuffer() const { return m_uvs; }

const EPrimitiveType CMesh::getPrimitiveType() const { return m_type; }

const std::unique_ptr<CVertexArrayObject>& CMesh::getVertexArray() const { return m_vao; }

GLenum CMesh::toGLPrimitive(EPrimitiveType type)
{
    switch (type)
    {
    case EPrimitiveType::Point:
        return GL_POINTS;
        break;
    case EPrimitiveType::Line:
        return GL_LINE;
        break;
    case EPrimitiveType::Triangle:
        return GL_TRIANGLES;
        break;
    default:
        LOG_ERROR("Invalid or unknown primitive type, default triangle type used.");
        // Default
        return GL_TRIANGLES;
    }
}

unsigned int CMesh::getPrimitiveSize(EPrimitiveType type)
{
    switch (type)
    {
    case EPrimitiveType::Point:
        return 1;
        break;
    case EPrimitiveType::Line:
        return 2;
        break;
    case EPrimitiveType::Triangle:
        return 3;
        break;
    default:
        LOG_ERROR("Invalid or unknown primitive type");
        return 0;
    }
}
