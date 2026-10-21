#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/gl.h>

/**
 * \brief The face of a cubemap and the coordinates within it.
 */
struct CubemapCoordinates
{
    int face = 0;   /**< The face, in the order of the GL_TEXTURE_CUBE_MAP targets. */
    float s = 0.f;  /**< The horizontal coordinate within the face. */
    float t = 0.f;  /**< The vertical coordinate within the face. */
};

/**
 * \brief Returns the face and the coordinates of a cubemap for a direction.
 *
 * The direction does not have to be normalized.
 */
CubemapCoordinates cubemapCoordinatesForDirection(const glm::vec3& direction);

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
    GLuint m_textureId = 0;
    bool m_valid = false;
};
