#pragma once

#include <memory>
#include <vector>

#include "core/RendererCoreConfig.h"

class CTexture;
class CRenderBuffer;

/**
* \brief Represents a frame buffer (rendering target).
*/
class CFrameBuffer
{
   public:
    CFrameBuffer();
    ~CFrameBuffer();

    bool init();

    bool isValid() const;

    std::string getState();

    void setActive(GLenum target);
    void setInactive(GLenum target);

    void attach(const std::shared_ptr<CTexture>& texture, GLenum attachment);
    void attach(const std::shared_ptr<CRenderBuffer>& renderBuffer, GLenum attachment);

   private:
    std::vector<GLenum> m_drawBuffers; /**< Stores draw buffer attachments. */
    GLuint m_fboId;                    /**< Frame buffer id. */
    bool m_valid;                      /**< Frame buffer validity. */
};
