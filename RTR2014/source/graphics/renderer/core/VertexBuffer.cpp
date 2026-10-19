#include <spdlog/spdlog.h>
#include "graphics/renderer/core/VertexBuffer.h"

#include <cassert>


VertexBuffer::VertexBuffer(const std::vector<float>& data, GLenum usage)
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

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_bufferId); }

void VertexBuffer::setActive() const
{
    assert(isValid());
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
}

void VertexBuffer::setInactive() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

bool VertexBuffer::isValid() const { return m_valid; }

GLuint VertexBuffer::getId() const { return m_bufferId; }

unsigned int VertexBuffer::getSize() const { return m_size; }

VertexBuffer::VertexBuffer(GLenum usage)
    : m_bufferId(0), m_valid(false), m_size(0), m_usage(usage)
{
    glGenBuffers(1, &m_bufferId);
}

void VertexBuffer::setData(const std::vector<float>& data)
{
    // Unchecked bind
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
    // Set data
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), m_usage);
    setInactive();
    m_valid = true;
}
