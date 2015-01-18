#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

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

    GLuint getId() const { return m_fboId; };

    bool init();

    bool isValid() const;

    std::string getState();

    void setActive(GLenum target);
    void setInactive(GLenum target);

    void resize(unsigned int width, unsigned int height);

    void attach(const std::shared_ptr<CTexture>& texture, GLenum attachment);
    void attach(const std::shared_ptr<CRenderBuffer>& renderBuffer, GLenum attachment);

    static void setDefaultActive();

   private:
    std::vector<GLenum> m_drawBuffers; /**< Stores draw buffer attachments. */

    std::unordered_map<GLenum, std::shared_ptr<CTexture>>
        m_textures; /**< Stores attached textures. */
    std::unordered_map<GLenum, std::shared_ptr<CRenderBuffer>>
        m_renderBuffers; /**< Stores attached render buffers. */

    GLuint m_fboId; /**< Frame buffer id. */
    bool m_valid;   /**< Frame buffer validity. */
};
