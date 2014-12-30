#include "CRenderBuffer.h"

CRenderBuffer::CRenderBuffer() : m_bufferId(0), m_height(0), m_width(0) { return; }

CRenderBuffer::~CRenderBuffer()
{
    if (m_init)
    {
        glDeleteRenderbuffers(1, &m_bufferId);
    }
}

bool CRenderBuffer::init(unsigned int width, unsigned int height, GLenum format)
{
    if (m_init)
    {
        glDeleteRenderbuffers(1, &m_bufferId);
    }
    glGenRenderbuffers(1, &m_bufferId);
    setActive();
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    m_width = width;
    m_height = height;
    m_format = format;
    return true;
}

bool CRenderBuffer::resize(unsigned int width, unsigned int height)
{
    if (m_width != width || m_height != height)
    {
        init(width, height, getFormat());
        return true;
    }
    return false;
}

GLuint CRenderBuffer::getId() const { return m_bufferId; }

GLenum CRenderBuffer::getFormat() const { return m_format; }

void CRenderBuffer::setActive() const { glBindRenderbuffer(GL_RENDERBUFFER, m_bufferId); }