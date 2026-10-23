#include "graphics/Cubemap.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "util/ColorSpace.h"
#include "util/FatalError.h"

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

// The texels of the atlas are bytes, the conversion to linear is the same for
// every texel, so it is looked up instead of calculated.
const std::array<float, 256>& srgbBytesToLinear()
{
    static const std::array<float, 256> table = [] {
        std::array<float, 256> values{};
        for (int i = 0; i < 256; ++i)
        {
            values[i] = sRGBToLinear((float)i / 255.0f);
        }
        return values;
    }();
    return table;
}

}  // namespace

glm::vec3 cubemapFaceDirection(int face, float s, float t)
{
    switch (face)
    {
    case PositiveX:
        return glm::vec3(1.f, 1.f - 2.f * t, 1.f - 2.f * s);
    case NegativeX:
        return glm::vec3(-1.f, 1.f - 2.f * t, 2.f * s - 1.f);
    case PositiveY:
        return glm::vec3(2.f * s - 1.f, 1.f, 2.f * t - 1.f);
    case NegativeY:
        return glm::vec3(2.f * s - 1.f, -1.f, 1.f - 2.f * t);
    case PositiveZ:
        return glm::vec3(2.f * s - 1.f, 1.f - 2.f * t, 1.f);
    default:
        return glm::vec3(1.f - 2.f * s, 1.f - 2.f * t, -1.f);
    }
}

CubemapCoordinates cubemapCoordinatesForDirection(const glm::vec3& direction)
{
    const glm::vec3& d = direction;
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
        fatalError("The image is too small for a cubemap atlas of faces of {} pixels.", faceSize);
    }
    if (pixels.size() < static_cast<size_t>(width) * height * 3)
    {
        fatalError("The image does not hold the data of {}x{} pixels.", width, height);
    }

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_textureId);
    if (m_textureId == 0)
    {
        fatalError("Failed to create the cubemap.");
    }
    // The whole chain of mip levels has to be allocated, they are generated
    // for the faces afterwards. The faces hold a color, the hardware decodes
    // it when it is sampled.
    const unsigned int levels = 1 + (unsigned int)std::log2((float)faceSize);
    glTextureStorage2D(m_textureId, levels, GL_SRGB8, static_cast<GLsizei>(faceSize),
                       static_cast<GLsizei>(faceSize));

    // The faces of the cubemap, one after the other, the order the GL cube map
    // targets have.
    const size_t faceBytes = (size_t)faceSize * faceSize * 3;
    std::vector<unsigned char> faces(faceBytes * 6);
    for (int f = 0; f < 6; ++f)
    {
        // The cells of the atlas are copied out one face at a time, the source
        // of an upload has to be contiguous.
        const glm::ivec2 cell = atlasCellForFace(f);
        for (unsigned int row = 0; row < faceSize; ++row)
        {
            const size_t source =
                ((size_t)(cell.y * faceSize + row) * width + cell.x * faceSize) * 3;
            std::copy_n(pixels.begin() + source, faceSize * 3,
                        faces.begin() + f * faceBytes + (size_t)row * faceSize * 3);
        }
        glTextureSubImage3D(m_textureId, 0, 0, 0, f, static_cast<GLsizei>(faceSize),
                            static_cast<GLsizei>(faceSize), 1, GL_RGB, GL_UNSIGNED_BYTE,
                            faces.data() + f * faceBytes);
    }

    // The mip levels are generated here and not by the driver. The driver
    // generates them per face, the content of a face border does not match the
    // neighbouring face and the seams of the cube show.
    generateMipLevels(faces, faceSize, levels);

    glTextureParameteri(m_textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_valid = true;
    return true;
}

void Cubemap::generateMipLevels(const std::vector<unsigned char>& base, unsigned int faceSize,
                                unsigned int levels)
{
    // The level that is written and the one it is read from, they take turns.
    std::vector<unsigned char> current = base;
    std::vector<unsigned char> previous;

    for (unsigned int level = 1; level < levels; ++level)
    {
        const unsigned int size = faceSize >> level;
        const unsigned int previousSize = size * 2;
        const size_t levelBytes = (size_t)size * size * 3;
        const size_t previousLevelBytes = (size_t)previousSize * previousSize * 3;

        previous.swap(current);
        current.assign(6 * levelBytes, 0);
        const std::array<float, 256>& toLinear = srgbBytesToLinear();

        for (int f = 0; f < 6; ++f)
        {
            const unsigned char* source = previous.data() + f * previousLevelBytes;
            for (unsigned int y = 0; y < size; ++y)
            {
                for (unsigned int x = 0; x < size; ++x)
                {
                    const size_t top = (size_t)(y * 2) * previousSize + x * 2;
                    const size_t bottom = top + previousSize;
                    glm::vec3 sum(0.0f);
                    for (int c = 0; c < 3; ++c)
                    {
                        sum[c] = toLinear[source[(top + 0) * 3 + c]] +
                                 toLinear[source[(top + 1) * 3 + c]] +
                                 toLinear[source[(bottom + 0) * 3 + c]] +
                                 toLinear[source[(bottom + 1) * 3 + c]];
                    }
                    // The level is stored sRGB encoded, like the base level.
                    const glm::vec3 encoded = linearToSRGB(sum / 4.0f);
                    const size_t offset = f * levelBytes + ((size_t)y * size + x) * 3;
                    for (int c = 0; c < 3; ++c)
                    {
                        current[offset + c] =
                            (unsigned char)std::clamp(encoded[c] * 255.0f + 0.5f, 0.0f, 255.0f);
                    }
                }
            }
            glTextureSubImage3D(m_textureId, (GLint)level, 0, 0, f, static_cast<GLsizei>(size),
                                static_cast<GLsizei>(size), 1, GL_RGB, GL_UNSIGNED_BYTE,
                                current.data() + f * levelBytes);
        }
    }
}

glm::vec3 sampleCubemapFace(const unsigned char* faces, unsigned int size, int face, float s,
                            float t)
{
    const unsigned char* data = faces + (size_t)face * size * size * 3;
    const float x = s * size - 0.5f;
    const float y = t * size - 0.5f;
    const int x0 = (int)std::floor(x);
    const int y0 = (int)std::floor(y);
    const int x1 = std::min(x0 + 1, (int)size - 1);
    const int y1 = std::min(y0 + 1, (int)size - 1);
    const int cx0 = std::clamp(x0, 0, (int)size - 1);
    const int cy0 = std::clamp(y0, 0, (int)size - 1);
    const float fx = x - (float)cx0;
    const float fy = y - (float)cy0;

    // The texels are sRGB encoded, the filtering happens in linear space.
    const std::array<float, 256>& toLinear = srgbBytesToLinear();
    glm::vec3 texel(0.0f);
    for (int c = 0; c < 3; ++c)
    {
        const float a = toLinear[data[((size_t)cy0 * size + cx0) * 3 + c]];
        const float b = toLinear[data[((size_t)cy0 * size + x1) * 3 + c]];
        const float d = toLinear[data[((size_t)y1 * size + cx0) * 3 + c]];
        const float e = toLinear[data[((size_t)y1 * size + x1) * 3 + c]];
        texel[c] = a * (1 - fx) * (1 - fy) + b * fx * (1 - fy) + d * (1 - fx) * fy + e * fx * fy;
    }
    return texel;
}

void Cubemap::setActive(GLint textureUnit) const
{
    if (!m_valid)
    {
        fatalError("An invalid cubemap cannot be bound.");
    }
    glBindTextureUnit(textureUnit, m_textureId);
}

GLuint Cubemap::getId() const { return m_textureId; }

bool Cubemap::isValid() const { return m_valid; }
