#include "CFrameBuffer.h"

#include <cassert>

#include "graphics/resource/CTexture.h"
#include "graphics/renderer/CRenderBuffer.h"

CFrameBuffer::CFrameBuffer() : m_fboId(0), m_valid(false) { init(); }

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

bool CFrameBuffer::isValid() const { return m_valid; }

std::string CFrameBuffer::getState()
{
	assert(m_valid);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    GLenum state = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    switch (state)
    {
    case GL_FRAMEBUFFER_UNDEFINED:
        return "The current FBO binding is 0, but no default framebuffer exists.";
        break;
    case GL_FRAMEBUFFER_COMPLETE:
        return "A user-defined FBO is bound and is complete. OK to render.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "One of the buffers enabled for rendering is incomplete.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "No buffers are attached to the FBO.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "One of the buffer attachments enabled for rendering does not have a buffer "
               "attached.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "One of the buffer attachments enabled for reading does not have a buffer attached.";
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "The combination of internal buffer formats is not supported.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "The number of samples or the value for TEXTURE_FIXED_SAMPLE_LOCATIONS for all "
               "buffers does not match.";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "Not all color attachments are layered textures or bound to the same target.";
    }
    return "Invalid state encountered.";
}

void CFrameBuffer::setActive(GLenum target)
{
    assert(m_valid);
    glBindFramebuffer(target, m_fboId);
	// Set draw buffers
	if (!m_drawBuffers.empty())
	{
		glDrawBuffers((GLsizei) m_drawBuffers.size(), m_drawBuffers.data());
	}
}

void CFrameBuffer::setInactive(GLenum target) { glBindFramebuffer(target, 0); }

void CFrameBuffer::attach(const std::shared_ptr<CTexture>& texture, GLenum attachment)
{
	// Bind
	assert(m_valid);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    // Attach
	glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);
	// Add color attachments to draw buffers
	if (attachment != GL_DEPTH_ATTACHMENT && attachment != GL_STENCIL_ATTACHMENT && attachment != GL_DEPTH_STENCIL_ATTACHMENT)
	{
		m_drawBuffers.push_back(attachment);
	}
    setInactive(GL_FRAMEBUFFER);
}

void CFrameBuffer::attach(const std::shared_ptr<CRenderBuffer>& renderBuffer, GLenum attachment)
{
	// Bind
	assert(m_valid);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
	// Attach
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderBuffer->getId());
	// Add color attachments to draw buffers
	if (attachment != GL_DEPTH_ATTACHMENT && attachment != GL_STENCIL_ATTACHMENT && attachment != GL_DEPTH_STENCIL_ATTACHMENT)
	{
		m_drawBuffers.push_back(attachment);
	}
	setInactive(GL_FRAMEBUFFER);
}