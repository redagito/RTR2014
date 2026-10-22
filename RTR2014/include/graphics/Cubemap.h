#pragma once

#include <glad/gl.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

/**
 * \brief The face of a cubemap and the coordinates within it.
 */
struct CubemapCoordinates
{
    int face = 0;  /**< The face, in the order of the GL_TEXTURE_CUBE_MAP targets. */
    float s = 0.f; /**< The horizontal coordinate within the face. */
    float t = 0.f; /**< The vertical coordinate within the face. */
};

/**
 * \brief Returns the face and the coordinates of a cubemap for a direction.
 *
 * The direction does not have to be normalized.
 */
CubemapCoordinates cubemapCoordinatesForDirection(const glm::vec3& direction);

/**
 * \brief Returns the direction of the texel at the coordinates of a face.
 *
 * The inverse of cubemapCoordinatesForDirection.
 */
glm::vec3 cubemapFaceDirection(int face, float s, float t);

/**
 * \brief Returns the cell of a horizontal cross atlas that holds a face.
 *
 * The cells are given as (column, row) in the image, the row 0 is the top.
 */
glm::ivec2 atlasCellForFace(int face);

/**
 * \brief Returns the coordinates within the atlas cell of a face.
 *
 * The horizontal cross atlas stores the faces in the orientation of the cubemap
 * they belong to, only the face of the negative x axis is mirrored.
 */
glm::vec2 atlasCoordinatesForFace(int face, float s, float t);

/**
 * \brief Samples a face of the cubemap at the given coordinates.
 *
 * The faces are given as one buffer, one face after the other, with three
 * bytes per texel. The values are sRGB encoded, they are returned in linear
 * space, so that the filtering of the mip levels happens in linear space.
 */
glm::vec3 sampleCubemapFace(const unsigned char* faces, unsigned int size, int face, float s,
                            float t);

/**
 * \brief A cubemap, the six faces of an environment.
 */
class Cubemap
{
   public:
    Cubemap();
    ~Cubemap();

    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;

    /**
     * \brief Creates the cubemap from a horizontal cross atlas.
     *
     * The atlas holds the six faces in a cross of square cells, the cell of the
     * positive y axis is in the top row and the cell of the negative y axis in
     * the bottom row, the other four are in the middle row.
     */
    bool initFromAtlas(const std::vector<unsigned char>& pixels, unsigned int width,
                       unsigned int height, unsigned int faceSize);

    /**
     * \brief Binds the cubemap to a texture unit.
     */
    void setActive(GLint textureUnit) const;

    /**
     * \brief Returns the texture id.
     */
    GLuint getId() const;

    /**
     * \brief Returns whether the cubemap is usable.
     */
    bool isValid() const;

   private:
    /**
     * \brief Generates the mip levels of the cubemap.
     *
     * The levels are filtered across the face borders, the levels the driver
     * generates are clamped to the face and show the seams of the cube. The
     * faces are given as one buffer, one after the other.
     */
    void generateMipLevels(const std::vector<unsigned char>& base, unsigned int faceSize,
                           unsigned int levels);

    GLuint m_textureId = 0;
    bool m_valid = false;
};
