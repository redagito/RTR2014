#include "CTexture.h"

#include <cassert>

CTexture::CTexture(const std::vector<unsigned int>& image, unsigned int width, unsigned int height,
                   EColorFormat format, unsigned int mipmapLevel)
    : m_textureId(0), m_width(width), m_height(height), m_format(format)
{
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    GLint internalFormat;
    switch (format)
    {
    case EColorFormat::RGB24:
        internalFormat = GL_RGB;
        break;
    case EColorFormat::RGBA32:
        internalFormat = GL_RGBA;
        break;
    default:
        assert(false && "Invalid or unknown texture format");
        break;
    }

    // Set filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    // Load data
    glTexImage2D(GL_TEXTURE_2D, mipmapLevel, internalFormat, m_width, m_height, 0, internalFormat,
                 GL_UNSIGNED_BYTE, image.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

CTexture::~CTexture() { glDeleteTextures(1, &m_textureId); }
