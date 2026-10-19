#include <spdlog/spdlog.h>
#include "graphics/Mesh.h"

#include "graphics/renderer/core/RendererCoreConfig.h"

#include <cassert>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
             const std::vector<float>& normals, const std::vector<float>& uvs, PrimitiveType type)
    : m_vertices(nullptr),
      m_indices(nullptr),
      m_normals(nullptr),
      m_uvs(nullptr),
      m_vao(nullptr),
      m_type(PrimitiveType::Invalid)
{
    init(vertices, indices, normals, uvs, type);
}

Mesh::~Mesh() { return; }

bool Mesh::init(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
                 const std::vector<float>& normals, const std::vector<float>& uvs,
                 PrimitiveType type)
{
    if (vertices.empty() || type == PrimitiveType::Invalid)
    {
        return false;
    }
    // Set vertex data
    m_vertices.reset(new VertexBuffer(vertices));

    if (!indices.empty())
    {
        // Set indices
        m_indices.reset(new IndexBuffer(indices));
    }

    if (!normals.empty())
    {
        // Set normals
        m_normals.reset(new VertexBuffer(normals));
    }

    if (!uvs.empty())
    {
        // Set uvs
        m_uvs.reset(new VertexBuffer(uvs));
    }

    // Create new vertex array object to store buffer state
    m_vao.reset(new VertexArrayObject);

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


    return true;
}

bool Mesh::hasIndexBuffer() const { return m_indices != nullptr; }

const std::unique_ptr<VertexBuffer>& Mesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<IndexBuffer>& Mesh::getIndexBuffer() const { return m_indices; }

const std::unique_ptr<VertexBuffer>& Mesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<VertexBuffer>& Mesh::getUVBuffer() const { return m_uvs; }

const PrimitiveType Mesh::getPrimitiveType() const { return m_type; }

const std::unique_ptr<VertexArrayObject>& Mesh::getVertexArray() const { return m_vao; }

GLenum Mesh::toGLPrimitive(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::Point:
        return GL_POINTS;
        break;
    case PrimitiveType::Line:
        return GL_LINE;
        break;
    case PrimitiveType::Triangle:
        return GL_TRIANGLES;
        break;
    default:
        spdlog::error("Invalid or unknown primitive type, default triangle type used.");
        // Default
        return GL_TRIANGLES;
    }
}

unsigned int Mesh::getPrimitiveSize(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::Point:
        return 1;
        break;
    case PrimitiveType::Line:
        return 2;
        break;
    case PrimitiveType::Triangle:
        return 3;
        break;
    default:
        spdlog::error("Invalid or unknown primitive type");
        return 0;
    }
}
