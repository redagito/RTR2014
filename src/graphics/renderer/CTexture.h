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
    CTexture(const std::vector<unsigned int>& imageData, unsigned int width, unsigned int height,
             EColorFormat format, unsigned int mipmapLevel = 0);

    ~CTexture();

   private:
    GLuint m_textureId;
    unsigned int m_width;
    unsigned int m_height;
    EColorFormat m_format;
};
