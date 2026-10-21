#include <spdlog/spdlog.h>
#include "graphics/Cubemap.h"

#include <algorithm>
#include <cmath>

namespace
{
// The faces of a cubemap, in the order of the GL_TEXTURE_CUBE_MAP targets.
enum Face
{
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5
};
} // namespace

CubemapCoordinates cubemapCoordinatesForDirection(const glm::vec3& direction)
{
    const glm::vec3 d = glm::normalize(direction);
    const float ax = std::fabs(d.x);
    const float ay = std::fabs(d.y);
    const float az = std::fabs(d.z);

    CubemapCoordinates coordinates;
    if (ax >= ay && ax >= az)
    {
        // The faces of the x axis, the coordinates run along z and y. The face
        // of the negative x axis runs along z the other way, so that the four
        // faces of the horizon form a panorama.
        coordinates.face = d.x > 0.f ? PositiveX : NegativeX;
        const float major = ax;
        const float sc = d.x > 0.f ? -d.z : d.z;
        coordinates.s = (sc / major + 1.f) * 0.5f;
        coordinates.t = (-d.y / major + 1.f) * 0.5f;
    }
    else if (ay >= az)
    {
        // The faces of the y axis, the coordinates run along x and z.
        coordinates.face = d.y > 0.f ? PositiveY : NegativeY;
        const float major = ay;
        const float tc = d.y > 0.f ? d.z : -d.z;
        coordinates.s = (d.x / major + 1.f) * 0.5f;
        coordinates.t = (tc / major + 1.f) * 0.5f;
    }
    else
    {
        // The faces of the z axis, the coordinates run along x and y.
        coordinates.face = d.z > 0.f ? PositiveZ : NegativeZ;
        const float major = az;
        const float sc = d.z > 0.f ? d.x : -d.x;
        coordinates.s = (sc / major + 1.f) * 0.5f;
        coordinates.t = (-d.y / major + 1.f) * 0.5f;
    }
    return coordinates;
}

glm::ivec2 atlasCellForFace(int face)
{
    switch (face)
    {
    case PositiveX:
        return glm::ivec2(2, 1);
    case NegativeX:
        return glm::ivec2(0, 1);
    case PositiveY:
        return glm::ivec2(1, 0);
    case NegativeY:
        return glm::ivec2(1, 2);
    case PositiveZ:
        return glm::ivec2(1, 1);
    case NegativeZ:
        return glm::ivec2(3, 1);
    default:
        return glm::ivec2(0, 0);
    }
}

glm::vec2 atlasCoordinatesForFace(int face, float s, float t)
{
    // The faces of the cross are stored in the orientation of the cubemap they
    // belong to, none of them is mirrored.
    return glm::vec2(s, t);
}

Cubemap::Cubemap() : m_textureId(0), m_valid(false) {}

Cubemap::~Cubemap()
{
    if (m_valid)
    {
        glDeleteTextures(1, &m_textureId);
    }
}

bool Cubemap::initFromAtlas(const std::vector<unsigned char>& pixels, unsigned int width,
                            unsigned int height, unsigned int faceSize)
{
    if (faceSize == 0 || width < faceSize * 4 || height < faceSize * 3)
    {
        spdlog::error("The image is too small for a cubemap atlas of faces of {} pixels.", faceSize);
        return false;
    }
    if (pixels.size() < static_cast<size_t>(width) * height * 3)
    {
        spdlog::error("The image does not hold the data of {}x{} pixels.", width, height);
        return false;
    }

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_textureId);
    if (m_textureId == 0)
    {
        spdlog::error("Failed to create the cubemap.");
        return false;
    }
    glTextureStorage2D(m_textureId, 1, GL_RGB8, static_cast<GLsizei>(faceSize),
                       static_cast<GLsizei>(faceSize));

    // The cells of the atlas are copied out one face at a time, the source of
    // an upload has to be contiguous.
    std::vector<unsigned char> face(faceSize * faceSize * 3);
    for (int f = 0; f < 6; ++f)
    {
        const glm::ivec2 cell = atlasCellForFace(f);
        for (unsigned int row = 0; row < faceSize; ++row)
        {
            const size_t source = ((size_t)(cell.y * faceSize + row) * width + cell.x * faceSize) * 3;
            std::copy_n(pixels.begin() + source, faceSize * 3, face.begin() + (size_t)row * faceSize * 3);
        }
        glTextureSubImage3D(m_textureId, 0, 0, 0, f, static_cast<GLsizei>(faceSize),
                            static_cast<GLsizei>(faceSize), 1, GL_RGB, GL_UNSIGNED_BYTE, face.data());
    }

    // The mipmaps are generated per face, the atlas would bleed between them.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTextureParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_valid = true;
    return true;
}

void Cubemap::setActive(GLint textureUnit) const
{
    if (!m_valid)
    {
        spdlog::error("An invalid cubemap cannot be bound.");
        return;
    }
    glBindTextureUnit(textureUnit, m_textureId);
}

GLuint Cubemap::getId() const { return m_textureId; }

bool Cubemap::isValid() const { return m_valid; }
