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
    CTexture(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
             EColorFormat format, unsigned int mipmapLevel = 0);

    ~CTexture();

	/**
	* \brief Initializes texture data.
	*/
	bool init(const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
		EColorFormat format, unsigned int mipmapLevel = 0);

	/**
	* \brief Returns texture id.
	*/
	GLuint getId() const;

   private:
    GLuint m_textureId;
    unsigned int m_width;
    unsigned int m_height;
    EColorFormat m_format;
};
