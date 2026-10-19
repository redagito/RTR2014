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
    glGenBuffers(1, &m_bufferId);
    // Unchecked bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
    // Set data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 usage);
    m_size = (unsigned int)indices.size();
    // TODO Check error
    // Unbind
    setInactive();

    m_valid = true;
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_bufferId); }

void IndexBuffer::setActive() const
{
    assert(isValid());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}

void IndexBuffer::setInactive() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

bool IndexBuffer::isValid() const { return m_valid; }

GLuint IndexBuffer::getId() const { return m_bufferId; }

unsigned int IndexBuffer::getSize() const { return m_size; }
