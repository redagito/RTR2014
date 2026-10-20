#pragma once

#include <vector>

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/ResourceConfig.h"

/**
 * \brief Texture class.
 */
class Texture
{
   public:
    Texture();

    /**
     * \brief Create from image data.
     */
    Texture(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
             ColorFormat format, bool createMipmaps = true);

    /**
     * \brief Creates empty texture.
     */
    Texture(unsigned int width, unsigned int height, ColorFormat format,
             bool createMipmaps = true);

    // TODO remove this and introduce ability to create cubetextures (necessary for point light
    // shadows)
    Texture(GLint id, bool hasMipmaps, unsigned int width, unsigned int height, GLint format,
             GLenum externalFormat);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    /**
    * \brief Initializes texture data.
    */
    bool init(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
              ColorFormat format, bool createMipmaps = true);

    bool init(unsigned int width, unsigned int height, GLint format);

    /**
    * \brief Resizes texture.
    */
    void resize(unsigned int width, unsigned int height);

    /**
    * \brief Enables the comparison mode, it is used for shadow maps.
    */
    void setCompareMode(bool enabled);

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

    /**
    * \brief Saves texture data as png.
    */
    void saveAsPng(const std::string& file);

   protected:
    bool init(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
              GLint format, bool createMipmaps);

   private:
    /**
     * \brief Applies the filtering and wrap parameters to a texture object.
     */
    void applyParameters(GLuint textureId, unsigned int width, unsigned int height) const;

    bool m_valid;
    bool m_hasMipmaps = false;
    bool m_compareMode = false; /**< Whether the texture is sampled with a comparison. */
    GLuint m_textureId;
    unsigned int m_width;
    unsigned int m_height;
    GLint m_format;
    GLenum m_externalFormat = GL_RGBA;
    GLenum m_externalType = GL_UNSIGNED_BYTE;
    GLenum m_internalFormat = GL_RGBA8; /**< Sized internal format of the storage. */
    unsigned int m_levels = 1;         /**< Number of allocated mipmap levels. */
};
