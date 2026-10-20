#include <spdlog/spdlog.h>
#include "graphics/renderer/core/IndexBuffer.h"

#include <cassert>


IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices, GLenum usage)
    : m_bufferId(0), m_size(0), m_valid(false)
{
    if (indices.empty())
    {
        spdlog::error("Index buffer data is empty.");
        return;
    }
    glCreateBuffers(1, &m_bufferId);
    if (m_bufferId == 0)
    {
        spdlog::error("Failed to create the index buffer.");
        return;
    }
    // Set data
    glNamedBufferData(m_bufferId, indices.size() * sizeof(unsigned int), indices.data(), usage);
    m_size = (unsigned int)indices.size();

    m_valid = true;
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_bufferId); }

bool IndexBuffer::isValid() const { return m_valid; }

GLuint IndexBuffer::getId() const { return m_bufferId; }

unsigned int IndexBuffer::getSize() const { return m_size; }
