#include "graphics/renderer/FrameBuffer.h"

#include <spdlog/spdlog.h>

#include <algorithm>

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

std::string FrameBuffer::getState() const
{
    if (!m_valid)
    {
        return "The frame buffer is not initialized";
    }
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
    if (!m_valid)
    {
        spdlog::error("An invalid frame buffer cannot be bound.");
        return;
    }
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

namespace
{
bool isColorAttachment(GLenum attachment)
{
    return attachment != GL_DEPTH_ATTACHMENT && attachment != GL_STENCIL_ATTACHMENT &&
           attachment != GL_DEPTH_STENCIL_ATTACHMENT;
}
}  // namespace

void FrameBuffer::attach(const std::shared_ptr<Texture>& texture, GLenum attachment)
{
    if (!m_valid)
    {
        spdlog::error("An invalid frame buffer cannot have an attachment.");
        return;
    }
    // Attach
    glNamedFramebufferTexture(m_fboId, attachment, texture->getId(), 0);
    m_textures[attachment] = texture;
    updateDrawBuffers();
}

void FrameBuffer::updateDrawBuffers()
{
    // The color attachments are the draw buffers. They are rebuilt because an
    // attachment can be replaced by another one.
    m_drawBuffers.clear();
    for (const auto& entry : m_textures)
    {
        if (isColorAttachment(entry.first))
        {
            m_drawBuffers.push_back(entry.first);
        }
    }
    for (const auto& entry : m_renderBuffers)
    {
        if (isColorAttachment(entry.first) && m_textures.count(entry.first) == 0)
        {
            m_drawBuffers.push_back(entry.first);
        }
    }
    std::sort(m_drawBuffers.begin(), m_drawBuffers.end());
}

void FrameBuffer::setDefaultActive() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::attach(const std::shared_ptr<RenderBuffer>& renderBuffer, GLenum attachment)
{
    if (!m_valid)
    {
        spdlog::error("An invalid frame buffer cannot have an attachment.");
        return;
    }
    // Attach
    glNamedFramebufferRenderbuffer(m_fboId, attachment, GL_RENDERBUFFER, renderBuffer->getId());
    m_renderBuffers[attachment] = renderBuffer;
    updateDrawBuffers();
}
