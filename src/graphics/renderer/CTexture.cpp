#include "CTexture.h"

#include <cassert>

CTexture::CTexture(const std::vector<unsigned char>& image, unsigned int width, unsigned int height,
                   EColorFormat format, unsigned int mipmapLevel)
    : m_valid(false), m_textureId(0), m_width(width), m_height(height), m_format(format)
{
	// Init texture with data
    init(image, width, height, format, mipmapLevel);
}

CTexture::~CTexture() { glDeleteTextures(1, &m_textureId); }

bool CTexture::init(const std::vector<unsigned char>& image, unsigned int width,
                    unsigned int height, EColorFormat format, unsigned int mipmapLevel)
{
    // Sanity checks
    if (width == 0 || height == 0 || image.empty() || format == EColorFormat::Invalid)
    {
        return false;
    }

    // Set format
    unsigned int bytePerPixel = 0;
    GLint internalFormat;
    switch (format)
    {
    case EColorFormat::GreyScale8:
        internalFormat = GL_R8;
        bytePerPixel = 1;
        break;
    case EColorFormat::RGB24:
        internalFormat = GL_RGB;
        bytePerPixel = 3;
        break;
    case EColorFormat::RGBA32:
        bytePerPixel = 4;
        internalFormat = GL_RGBA;
        break;
    default:
		return false;
        break;
    }

	// Size check
	if (image.size() != width * height * bytePerPixel)
	{
		return false;
	}

	// Create id
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// TODO Filter should be based on arguments and mip map level
    // Set filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    // Load data
    glTexImage2D(GL_TEXTURE_2D, mipmapLevel, internalFormat, m_width, m_height, 0, internalFormat,
                 GL_UNSIGNED_BYTE, image.data());
    // Mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    // Clean up previously created id
    if (m_textureId != 0)
    {
        glDeleteTextures(1, &m_textureId);
    }

    // Set new texture id
    m_textureId = textureId;
	m_valid = true;
    return true;
}

GLuint CTexture::getId() const
{
	return m_textureId;
}

bool CTexture::isValid() const
{
	return m_valid;
}