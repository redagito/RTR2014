#include "CIndexBuffer.h"

CIndexBuffer::CIndexBuffer(const std::vector<unsigned int>& indices, GLenum usage) : m_bufferId(0), m_size(0), m_valid(false)
{
	if (indices.empty())
	{
		return;
	}
    glGenBuffers(1, &m_bufferId);
	setActive();
	// Set data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);
	m_size = indices.size();
	// TODO Check error
	m_valid = true;
	// Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

CIndexBuffer::~CIndexBuffer() { glDeleteBuffers(1, &m_bufferId); }

void CIndexBuffer::setActive() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}

bool CIndexBuffer::isValid() const
{
	return m_valid;
}

GLuint CIndexBuffer::getId() const
{
	return m_bufferId;
}

unsigned int CIndexBuffer::getSize() const
{
	return m_size;
}