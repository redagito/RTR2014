#include "CTexture.h"

#include <cassert>
#include <string>

#include "graphics/renderer/debug/RendererDebug.h"
#include "debug/Log.h"

CTexture::CTexture()
    : m_valid(false), m_textureId(0), m_width(0), m_height(0), m_format(EColorFormat::Invalid)
{
    // empty
}

CTexture::CTexture(const std::vector<unsigned char>& image, unsigned int width, unsigned int height,
                   EColorFormat format, bool createMipmaps)
    : m_valid(false), m_textureId(0), m_width(0), m_height(0), m_format(format)
{
    // Init texture with data
    if (!init(image, width, height, format, createMipmaps))
    {
        LOG_ERROR("Failed to initialize texture.");
    }
}

CTexture::CTexture(unsigned int width, unsigned int height, EColorFormat format, bool createMipmaps)
    : m_valid(false), m_textureId(0), m_width(0), m_height(0), m_format(format)
{
    // Init texture with data
    if (!init({}, width, height, format, createMipmaps))
    {
        LOG_ERROR("Failed to initialize texture.");
    }
}

CTexture::~CTexture()
{
    if (m_valid)
    {
        glDeleteTextures(1, &m_textureId);
    }
}

bool CTexture::init(const std::vector<unsigned char>& image, unsigned int width,
                    unsigned int height, EColorFormat format, bool createMipmaps)
{
    // Set format
    GLint internalFormat;
    switch (format)
    {
    case EColorFormat::GreyScale8:
        internalFormat = GL_R8;
        break;
    case EColorFormat::RGB24:
        internalFormat = GL_RGB8;
        break;
    case EColorFormat::RGBA32:
        internalFormat = GL_RGBA8;
        break;
    default:
        return false;
        break;
    }
    return init(image, width, height, internalFormat, createMipmaps);
}

bool CTexture::init(unsigned int width, unsigned int height, GLint format)
{
    return init({}, width, height, format, false);
}

GLuint CTexture::getId() const { return m_textureId; }

bool CTexture::isValid() const { return m_valid; }

void CTexture::setActive(GLint textureUnit) const
{
    assert(isValid());
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

bool CTexture::init(const std::vector<unsigned char>& image, unsigned int width,
                    unsigned int height, GLint format, bool createMipmaps)
{
    // Sanity checks
    if (width == 0 || height == 0)
    {
        return false;
    }

    // Set format
    unsigned int bytePerPixel = 0;
    GLenum externalFormat;
    GLenum type;

    // Set external format and type, based on texture format
    switch (format)
    {
    case GL_R8:  // Red component only, used for greyscale
        externalFormat = GL_RED;
        type = GL_UNSIGNED_BYTE;
        bytePerPixel = 1;
        break;
    case GL_RG8:  // Red and green component
        externalFormat = GL_RG;
        type = GL_UNSIGNED_BYTE;
        bytePerPixel = 2;
        break;
    case GL_RGB8:  // RGB texture with values from 0-255
        externalFormat = GL_RGB;
        type = GL_UNSIGNED_BYTE;
        bytePerPixel = 3;
        break;
    case GL_DEPTH_COMPONENT24:  // Depth texture for FBO
        externalFormat = GL_DEPTH_COMPONENT;
        type = GL_UNSIGNED_BYTE;
        bytePerPixel = 3;
        break;
    case GL_RGBA8:  // RGB texture with alpha channel
        externalFormat = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        bytePerPixel = 4;
        break;
    case GL_RGB16F:  // RGB texture with half float precision, for hdr
        externalFormat = GL_RGB;
        type = GL_FLOAT;
        bytePerPixel = 6;
        break;
    case GL_RGBA16F:  // RGB texture with alpha channel with half float precision
        externalFormat = GL_RGBA;
        type = GL_FLOAT;
        bytePerPixel = 8;
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

    // TODO Filter should be based on arguments and mip map level
    // Set filters
    if (width == 1 && height == 1)
    {
        // Special case 1x1 texture
        // Needs nearest filtering otherwise texture is black?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        // Default value is linear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    // Set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    // Load data
    if (image.empty())
    {
        // No image data, only allocate texture space
        // TODO As of OpenGL 4.3, glTexStorage should be used for this.
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, externalFormat, type, nullptr);
    }
    else
    {
        // Load image data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, externalFormat, type,
                     image.data());
    }

    // Mipmaps
    // Do not generate for 1x1 images
    if (width != 1 && height != 1 && createMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // Clean up previously created id
    if (m_textureId != 0)
    {
        glDeleteTextures(1, &m_textureId);
    }

    // Set new texture id
    m_textureId = textureId;
    m_width = width;
    m_height = height;
    m_valid = true;
    return true;
}