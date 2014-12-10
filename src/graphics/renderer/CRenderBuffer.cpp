#include "CRenderBuffer.h"

CRenderBuffer::CRenderBuffer()
	:
	m_bufferId(0),
	m_height(0),
	m_width(0)
{
	glGenRenderbuffers(1, &m_bufferId);
	return;
}

CRenderBuffer::~CRenderBuffer()
{
	glDeleteRenderbuffers(1, &m_bufferId);
}

bool CRenderBuffer::init(unsigned int width, unsigned int height, GLenum format)
{
	setActive();
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	return true;
}

GLuint CRenderBuffer::getId() const
{
	return m_bufferId;
}

void CRenderBuffer::setActive() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_bufferId);
}