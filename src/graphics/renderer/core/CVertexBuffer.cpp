#include "CVertexBuffer.h"

CVertexBuffer::CVertexBuffer(const std::vector<float>& data, GLenum usage)
    : m_bufferId(0), m_valid(false), m_size(data.size())
{
    // Create GL buffer resource
    glGenBuffers(1, &m_bufferId);
    setActive();
    // Set data
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), usage);
	setInactive();
    // TODO Check error
    m_valid = true;
}

CVertexBuffer::~CVertexBuffer() { glDeleteBuffers(1, &m_bufferId); }

void CVertexBuffer::setActive() const { glBindBuffer(GL_ARRAY_BUFFER, m_bufferId); }

void CVertexBuffer::setInactive() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

bool CVertexBuffer::isValid() const { return m_valid; }

GLuint CVertexBuffer::getId() const { return m_bufferId; }

unsigned int CVertexBuffer::getSize() const { return m_size; }