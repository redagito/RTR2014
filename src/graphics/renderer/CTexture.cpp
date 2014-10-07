#include "CTexture.h"

#include "debug/RendererDebug.h"
#include "debug/Log.h"

#include <cassert>

CTexture::CTexture(const std::vector<unsigned char>& image, unsigned int width, unsigned int height,
                   EColorFormat format, unsigned int mipmapLevel)
    : m_valid(false), m_textureId(0), m_width(width), m_height(height), m_format(format)
{
    // Init texture with data
    if (!init(image, width, height, format, mipmapLevel))
	{ 
		LOG_ERROR("Failed to initialize texture.");
	}
}

CTexture::~CTexture() { glDeleteTextures(1, &m_textureId); }

bool CTexture::init(const std::vector<unsigned char>& image, unsigned int width,
                    unsigned int height, EColorFormat format, unsigned int mipmapLevel)
{
	// Error check
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

    // Sanity checks
    if (width == 0 || height == 0 || format == EColorFormat::Invalid)
    {
        return false;
    }

    // Set format
    unsigned int bytePerPixel = 0;
    GLint internalFormat;
	GLenum externalFormat;
    switch (format)
    {
    case EColorFormat::GreyScale8:
        internalFormat = GL_R8;
		externalFormat = GL_RED;
        bytePerPixel = 1;
        break;
    case EColorFormat::RGB24:
        internalFormat = GL_RGB8;
		externalFormat = GL_RGB;
        bytePerPixel = 3;
        break;
    case EColorFormat::RGBA32:
		internalFormat = GL_RGBA8;
		externalFormat = GL_RGBA;
		bytePerPixel = 4;
        break;
    default:
        return false;
        break;
    }

    // Image data is either empty or has correct size
    if (!image.empty() && (image.size() != width * height * bytePerPixel))
    {
        return false;
    }

    // Create id
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

    // TODO Filter should be based on arguments and mip map level
    // Set filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

    // Load data
    glTexImage2D(GL_TEXTURE_2D, mipmapLevel, internalFormat, m_width, m_height, 0, externalFormat,
                 GL_UNSIGNED_BYTE, image.data());
    
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

	// Mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
	
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}

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
	
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}
    return true;
}

GLuint CTexture::getId() const { return m_textureId; }

bool CTexture::isValid() const { return m_valid; }

void CTexture::setActive(GLint textureUnit) const
{
	assert(isValid());
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
}