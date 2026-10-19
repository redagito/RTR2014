#include "graphics/renderer/RenderBuffer.h"

RenderBuffer::RenderBuffer() : m_bufferId(0), m_height(0), m_width(0) { return; }

RenderBuffer::~RenderBuffer()
{
    if (m_init)
    {
        glDeleteRenderbuffers(1, &m_bufferId);
    }
}

bool RenderBuffer::init(unsigned int width, unsigned int height, GLenum format)
{
    if (m_init)
    {
        glDeleteRenderbuffers(1, &m_bufferId);
    }
    glCreateRenderbuffers(1, &m_bufferId);
    glNamedRenderbufferStorage(m_bufferId, format, width, height);
    m_width = width;
    m_height = height;
    m_format = format;
    m_init = true;
    return true;
}

bool RenderBuffer::resize(unsigned int width, unsigned int height)
{
    if (m_width != width || m_height != height)
    {
        init(width, height, getFormat());
        return true;
    }
    return false;
}

GLuint RenderBuffer::getId() const { return m_bufferId; }

GLenum RenderBuffer::getFormat() const { return m_format; }

