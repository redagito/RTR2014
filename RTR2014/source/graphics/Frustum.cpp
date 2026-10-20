#include "graphics/Frustum.h"

#include "resource/OrientedBoundingBox.h"

Frustum::Frustum(const glm::mat4& viewProjection)
{
    glm::vec4 row0(viewProjection[0][0], viewProjection[1][0], viewProjection[2][0],
                   viewProjection[3][0]);
    glm::vec4 row1(viewProjection[0][1], viewProjection[1][1], viewProjection[2][1],
                   viewProjection[3][1]);
    glm::vec4 row2(viewProjection[0][2], viewProjection[1][2], viewProjection[2][2],
                   viewProjection[3][2]);
    glm::vec4 row3(viewProjection[0][3], viewProjection[1][3], viewProjection[2][3],
                   viewProjection[3][3]);

    glm::vec4 planes[6] = {
        row3 + row0,  // Left
        row3 - row0,  // Right
        row3 + row1,  // Bottom
        row3 - row1,  // Top
        row3 + row2,  // Near
        row3 - row2   // Far
    };

    for (int i = 0; i < 6; ++i)
    {
        glm::vec3 normal(planes[i]);
        float length = glm::length(normal);
        if (length > 0.0f)
        {
            planes[i] /= length;
        }
        m_planes[i] = planes[i];
    }
}

bool Frustum::intersects(const OrientedBoundingBox& box) const
{
    for (int i = 0; i < 6; ++i)
    {
        if (!box.intersectsPlane(m_planes[i]))
        {
            return false;
        }
    }
    return true;
}
