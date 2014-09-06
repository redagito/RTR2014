#include "CMesh.h"
#include "debug/Log.h"

#include "core/RendererCoreConfig.h"

#include <cassert>

CMesh::CMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
             const std::vector<float>& normals, const std::vector<float>& uvs, EPrimitiveType type)
			 : m_vertices(nullptr), m_normals(nullptr), m_uvs(nullptr), m_type(EPrimitiveType::Invalid), m_vao(nullptr)
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

	m_vertices->setActive();
	// Set vertex data attributes
	// TODO The location should be in some kind of shader interface definition
	// TODO The data data size (second param) should be deduced by the data type of the provided vertex data
	//  basically we would need a std::vector<glm::vec3> instead of a std::vector<float> to deduce the size
	// TODO Consider packing all vertex data into a single buffer for better performance
	glVertexAttribPointer(vertexDataShaderLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set normal data attributes
	if (m_normals != nullptr)
	{
		m_normals->setActive();
		glVertexAttribPointer(normalDataShaderLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Set uv data attributes
	if (m_uvs != nullptr)
	{
		m_uvs->setActive();
		glVertexAttribPointer(uvDataShaderLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Disable vao
	m_vao->setInactive();
    return true;
}

const std::unique_ptr<CVertexBuffer>& CMesh::getVertexBuffer() const { return m_vertices; }

const std::unique_ptr<CIndexBuffer>& CMesh::getIndexBuffer() const { return m_indices; }

const std::unique_ptr<CVertexBuffer>& CMesh::getNormalBuffer() const { return m_normals; }

const std::unique_ptr<CVertexBuffer>& CMesh::getUVBuffer() const { return m_uvs; }

void CMesh::setActive() const
{
	m_vao->setActive();
}