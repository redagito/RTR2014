#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

#include <glm/glm.hpp>

#include "resource/ResourceManager.h"

namespace
{
// The cotangent frame of the geometry pass shader, evaluated on a triangle
// with the derivatives of its barycentric interpolation.
void tangentFrame(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                  const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
                  const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2, glm::vec3& t,
                  glm::vec3& b)
{
    const glm::vec3 dp1 = p1 - p0;
    const glm::vec3 dp2 = p2 - p0;
    const glm::vec2 duv1 = uv1 - uv0;
    const glm::vec2 duv2 = uv2 - uv0;
    const glm::vec3 n = glm::normalize(n0 + n1 + n2);

    const glm::vec3 dp2perp = glm::cross(dp2, n);
    const glm::vec3 dp1perp = glm::cross(n, dp1);
    t = dp2perp * duv1.x + dp1perp * duv2.x;
    b = dp2perp * duv1.y + dp1perp * duv2.y;
    const float invmax = 1.f / std::sqrt(std::max(glm::dot(t, t), glm::dot(b, b)));
    t *= invmax;
    b *= invmax;
}
} // namespace

TEST_CASE("the tangent frame points along the u axis of the texture", "[mesh]")
{
    ResourceManager manager;
    const ResourceId meshId = manager.loadMesh(RTR_PROJECT_DATA_DIR "/mesh/new_cave.obj");
    REQUIRE(meshId != invalidResource);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    PrimitiveType type = PrimitiveType::Invalid;
    REQUIRE(manager.getMesh(meshId, vertices, indices, normals, uvs, type));

    int alongU = 0;
    int againstU = 0;
    const size_t triangles = vertices.size() / 9;
    for (size_t i = 0; i < triangles; ++i)
    {
        glm::vec3 p[3];
        glm::vec3 n[3];
        glm::vec2 uv[3];
        for (int v = 0; v < 3; ++v)
        {
            p[v] = glm::vec3(vertices[i * 9 + v * 3], vertices[i * 9 + v * 3 + 1],
                             vertices[i * 9 + v * 3 + 2]);
            n[v] = glm::vec3(normals[i * 9 + v * 3], normals[i * 9 + v * 3 + 1],
                             normals[i * 9 + v * 3 + 2]);
            uv[v] = glm::vec2(uvs[i * 6 + v * 2], uvs[i * 6 + v * 2 + 1]);
        }

        // The corner with the smallest and the largest u, the direction of
        // increasing u.
        int lowU = 0;
        int highU = 0;
        for (int v = 1; v < 3; ++v)
        {
            if (uv[v].x < uv[lowU].x) { lowU = v; }
            if (uv[v].x > uv[highU].x) { highU = v; }
        }
        if (uv[highU].x - uv[lowU].x < 1e-4f) { continue; }
        const glm::vec3 directionOfU = p[highU] - p[lowU];

        glm::vec3 t, b;
        tangentFrame(p[0], p[1], p[2], n[0], n[1], n[2], uv[0], uv[1], uv[2], t, b);

        if (glm::dot(t, directionOfU) > 0.f) { ++alongU; } else { ++againstU; }
    }

    REQUIRE(alongU + againstU > 0);
    // The tangent has to point towards the increasing u, the normal map
    // stores its tilt along the tangent.
    REQUIRE(alongU > againstU * 9);
}
