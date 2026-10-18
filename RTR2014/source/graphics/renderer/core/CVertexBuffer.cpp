#include <spdlog/spdlog.h>
#include "graphics/renderer/core/CVertexBuffer.h"

#include <cassert>


CVertexBuffer::CVertexBuffer(const std::vector<float>& data, GLenum usage)
    : m_bufferId(0), m_valid(false), m_size((unsigned int)data.size()), m_usage(usage)
{
    if (data.empty())
    {
        spdlog::error("Vertex buffer data is empty.");
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

CVertexBuffer::CVertexBuffer(GLenum usage)
    : m_bufferId(0), m_valid(false), m_size(0), m_usage(usage)
{
    glGenBuffers(1, &m_bufferId);
}

void CVertexBuffer::setData(const std::vector<float>& data)
{
    // Unchecked bind
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    // Set data
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), m_usage);
    setInactive();
    m_valid = true;
}
