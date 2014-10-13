#pragma once

#include <vector>

#include "core/RendererCoreConfig.h"
#include "graphics/ResourceConfig.h"

/**
 * \brief Texture class.
 */
class CTexture
{
   public:
    CTexture();

    /**
     * \brief Create from image data.
     */
    CTexture(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
             EColorFormat format, bool createMipmaps = true);

    /**
     * \brief Creates empty texture.
     */
    CTexture(unsigned int width, unsigned int height, EColorFormat format,
             bool createMipmaps = true);

    ~CTexture();

    /**
    * \brief Initializes texture data.
    */
    bool init(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
              EColorFormat format, bool createMipmaps = true);

    bool init(unsigned int width, unsigned int height, GLint format);

    /**
    * \brief Returns texture id.
    */
    GLuint getId() const;

    /**
    * \brief Returns texture validity.
    */
    bool isValid() const;

    /**
    * \bref Sets texture active as texture unit.
    */
    void setActive(GLint textureUnit) const;

   protected:
    bool init(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
              GLint format, bool createMipmaps);

   private:
    bool m_valid;
    GLuint m_textureId;
    unsigned int m_width;
    unsigned int m_height;
    EColorFormat m_format;
};
