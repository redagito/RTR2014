#include "CVertexBuffer.h"

#include <cassert>

#include "graphics/renderer/debug/RendererDebug.h"
#include "debug/Log.h"

CVertexBuffer::CVertexBuffer(const std::vector<float>& data, GLenum usage)
    : m_bufferId(0), m_valid(false), m_size(data.size())
{
	if (data.empty())
	{
		LOG_ERROR("Vertex buffer data is empty.");
		return;
	}
    // Create GL buffer resource
	glGenBuffers(1, &m_bufferId);
	// Unchecked bind
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    // Set data
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), usage);
	setInactive();
    // TODO Check error
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
		return;
	}
    m_valid = true;
}

CVertexBuffer::~CVertexBuffer() { glDeleteBuffers(1, &m_bufferId); }

void CVertexBuffer::setActive() const 
{
	assert(isValid()); 
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferId); 
}

void CVertexBuffer::setInactive() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

bool CVertexBuffer::isValid() const { return m_valid; }

GLuint CVertexBuffer::getId() const { return m_bufferId; }

unsigned int CVertexBuffer::getSize() const { return m_size; }