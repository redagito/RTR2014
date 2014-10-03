#include "CFrameBuffer.h"

#include <cassert>

#include "CTexture.h"

CFrameBuffer::CFrameBuffer()
	:
	m_fboId(0),
	m_valid(false)
{
	init();
}

CFrameBuffer::~CFrameBuffer()
{
	if (m_valid)
	{
		glDeleteFramebuffers(1, &m_fboId);
	}
}

bool CFrameBuffer::init()
{
	GLuint bufferId;
	glGenFramebuffers(1, &bufferId);

	m_fboId = bufferId;
	m_valid = true;

	return m_valid;
}

void CFrameBuffer::setActive(GLenum target)
{
	assert(m_valid);
	glBindFramebuffer(target, m_fboId);
}

void CFrameBuffer::setInactive(GLenum target)
{
	glBindFramebuffer(target, 0);
}

void CFrameBuffer::attach(const std::shared_ptr<CTexture>& texture, GLenum attachment)
{
	setActive(GL_FRAMEBUFFER);
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);
	setInactive(GL_FRAMEBUFFER);
}