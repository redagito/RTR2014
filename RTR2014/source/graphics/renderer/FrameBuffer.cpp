#include "graphics/renderer/FrameBuffer.h"

#include <cassert>

#include "graphics/Texture.h"
#include "graphics/renderer/RenderBuffer.h"

FrameBuffer::FrameBuffer() : m_fboId(0), m_valid(false) { init(); }

FrameBuffer::~FrameBuffer()
{
    if (m_valid)
    {
        glDeleteFramebuffers(1, &m_fboId);
    }
}

bool FrameBuffer::init()
{
    GLuint bufferId;
    glCreateFramebuffers(1, &bufferId);

    m_fboId = bufferId;
    m_valid = true;

    return m_valid;
}

std::string FrameBuffer::getState()
{
    assert(m_valid);
    // Caller must rebind the framebuffer it was using; this leaves the default FBO bound.
    GLenum state = glCheckNamedFramebufferStatus(m_fboId, GL_FRAMEBUFFER);

    switch (state)
    {
    case GL_FRAMEBUFFER_UNDEFINED:
        return "The current FBO binding is 0, but no default framebuffer exists";

    case GL_FRAMEBUFFER_COMPLETE:
        return "A user-defined FBO is bound and is complete. OK to render";

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "One of the buffers enabled for rendering is incomplete";

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "No buffers are attached to the FBO.";

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "One of the buffer attachments enabled for rendering does not have a buffer "
               "attached";

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "One of the buffer attachments enabled for reading does not have a buffer attached";

    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "The combination of internal buffer formats is not supported";

    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "The number of samples or the value for TEXTURE_FIXED_SAMPLE_LOCATIONS for all "
               "buffers does not match";

    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "Not all color attachments are layered textures or bound to the same target";
    }
    return "Invalid state encountered";
}

void FrameBuffer::setActive(GLenum target)
{
    assert(m_valid);
    glBindFramebuffer(target, m_fboId);
    // Set draw buffers
    if (!m_drawBuffers.empty())
    {
        glNamedFramebufferDrawBuffers(m_fboId, (GLsizei)m_drawBuffers.size(), m_drawBuffers.data());
    }
}

void FrameBuffer::setInactive(GLenum target) { glBindFramebuffer(target, 0); }

void FrameBuffer::resize(unsigned int width, unsigned int height)
{
    for (auto& entry : m_textures)
    {
        entry.second->resize(width, height);
        // The storage reallocation invalidates the attachment
        glNamedFramebufferTexture(m_fboId, entry.first, entry.second->getId(), 0);
    }
    for (auto& entry : m_renderBuffers)
    {
		// Need renderbuffer connot be resized
		// Instead they are recreated and reattached
		if (entry.second->resize(width, height))
		{
			attach(entry.second, entry.first);
		}
    }
}

void FrameBuffer::attach(const std::shared_ptr<Texture>& texture, GLenum attachment)
{
    // Attach
    assert(m_valid);
    glNamedFramebufferTexture(m_fboId, attachment, texture->getId(), 0);
    // Add color attachments to draw buffers
    if (attachment != GL_DEPTH_ATTACHMENT && attachment != GL_STENCIL_ATTACHMENT &&
        attachment != GL_DEPTH_STENCIL_ATTACHMENT)
    {
        m_drawBuffers.push_back(attachment);
    }
    m_textures[attachment] = texture;
}

void FrameBuffer::setDefaultActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attach(const std::shared_ptr<RenderBuffer>& renderBuffer, GLenum attachment)
{
    // Attach
    assert(m_valid);
    glNamedFramebufferRenderbuffer(m_fboId, attachment, GL_RENDERBUFFER, renderBuffer->getId());
    // Add color attachments to draw buffers
    if (attachment != GL_DEPTH_ATTACHMENT && attachment != GL_STENCIL_ATTACHMENT &&
        attachment != GL_DEPTH_STENCIL_ATTACHMENT)
    {
        m_drawBuffers.push_back(attachment);
    }
    m_renderBuffers[attachment] = renderBuffer;
}
