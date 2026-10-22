#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <glm/glm.hpp>
#include <vector>

#include "graphics/Cubemap.h"

namespace
{
// The position of a texel in the atlas for a direction.
glm::vec2 atlasTexel(const glm::vec3& direction)
{
    const CubemapCoordinates coordinates = cubemapCoordinatesForDirection(direction);
    const glm::ivec2 cell = atlasCellForFace(coordinates.face);
    const glm::vec2 within =
        atlasCoordinatesForFace(coordinates.face, coordinates.s, coordinates.t);
    return glm::vec2(cell.x + within.x, cell.y + within.y);
}
}  // namespace

TEST_CASE("a direction on an axis selects the face of that axis", "[graphics]")
{
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(1.f, 0.f, 0.f)).face == 0);
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(-1.f, 0.f, 0.f)).face == 1);
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(0.f, 1.f, 0.f)).face == 2);
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(0.f, -1.f, 0.f)).face == 3);
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(0.f, 0.f, 1.f)).face == 4);
    REQUIRE(cubemapCoordinatesForDirection(glm::vec3(0.f, 0.f, -1.f)).face == 5);
}

TEST_CASE("the atlas cells of the faces are the ones of a cross", "[graphics]")
{
    // The cell of the positive y axis is in the top row, the cell of the
    // negative y axis in the bottom row and the other four in the middle row.
    REQUIRE(atlasCellForFace(2) == glm::ivec2(1, 0));
    REQUIRE(atlasCellForFace(3) == glm::ivec2(1, 2));
    REQUIRE(atlasCellForFace(1) == glm::ivec2(0, 1));
    REQUIRE(atlasCellForFace(4) == glm::ivec2(1, 1));
    REQUIRE(atlasCellForFace(0) == glm::ivec2(2, 1));
    REQUIRE(atlasCellForFace(5) == glm::ivec2(3, 1));
}

TEST_CASE("the direction of a texel and its coordinates agree", "[graphics]")
{
    // The two are inverses of each other, the mip levels are filtered with
    // them. The texels on the border of a face lie on the border of two
    // faces, there the direction does not name a single one.
    for (int face = 0; face < 6; ++face)
    {
        for (int i = 1; i < 4; ++i)
        {
            for (int j = 1; j < 4; ++j)
            {
                const float s = (float)i / 4.f;
                const float t = (float)j / 4.f;
                const glm::vec3 direction = cubemapFaceDirection(face, s, t);
                const CubemapCoordinates coordinates = cubemapCoordinatesForDirection(direction);
                REQUIRE(coordinates.face == face);
                REQUIRE(coordinates.s == Catch::Approx(s).margin(0.001f));
                REQUIRE(coordinates.t == Catch::Approx(t).margin(0.001f));
            }
        }
    }
}

TEST_CASE("the atlas of the cubemap is continuous around the horizon", "[graphics]")
{
    // The four faces of the horizon have to form a panorama without a seam, so
    // a step along the horizon may not jump across the atlas. The walk follows
    // the order of the faces in the atlas, the z axis towards the x axis.
    const int steps = 720;
    glm::vec2 previous = atlasTexel(glm::vec3(0.f, 0.f, 1.f));
    for (int i = 1; i <= steps; ++i)
    {
        const float angle = 6.28318530717958647692f * (float)i / (float)steps;
        const glm::vec3 direction(std::sin(angle), 0.f, std::cos(angle));
        const glm::vec2 current = atlasTexel(direction);
        float step = current.x - previous.x;
        // The panorama wraps around at the end of the atlas.
        if (step < -3.9f)
        {
            step += 4.f;
        }
        REQUIRE(std::fabs(step) < 0.05f);
        REQUIRE(std::fabs(current.y - previous.y) < 0.05f);
        previous = current;
    }
}

TEST_CASE("the atlas of the cubemap is continuous from the horizon to the zenith", "[graphics]")
{
    // The face of the y axis has to meet the faces of the horizon.
    const glm::vec2 below = atlasTexel(glm::normalize(glm::vec3(0.f, 1.f, 0.999f)));
    const glm::vec2 above = atlasTexel(glm::normalize(glm::vec3(0.f, 1.f, 1.001f)));
    REQUIRE(glm::length(below - above) < 0.05f);
}

TEST_CASE("the cubemap is filtered in linear space", "[graphics]")
{
    // A face of two columns, one black and one with the middle gray. The
    // sample in the middle of the face is the average of the two, in linear
    // space it is much darker than the average of the two encoded values.
    const unsigned int size = 2;
    std::vector<unsigned char> faces(size * size * 3, 128);
    for (unsigned int y = 0; y < size; ++y)
    {
        for (unsigned int x = 0; x < size; ++x)
        {
            const unsigned char value = (x == 0) ? 0 : 128;
            const size_t offset = (y * size + x) * 3;
            for (int c = 0; c < 3; ++c)
            {
                faces[offset + c] = value;
            }
        }
    }

    const glm::vec3 sample = sampleCubemapFace(faces.data(), size, 0, 0.5f, 0.5f);

    // The average of the encoded values is 0.25, the linear one is half of
    // the decoded gray.
    const float expected = std::pow(128.0f / 255.0f, 2.2f) / 2.0f;
    REQUIRE(sample.r == Catch::Approx(expected).margin(0.001f));
    REQUIRE(sample.g == Catch::Approx(expected).margin(0.001f));
    REQUIRE(sample.b == Catch::Approx(expected).margin(0.001f));
}
