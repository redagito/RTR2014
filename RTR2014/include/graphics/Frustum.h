#pragma once

#include <glm/glm.hpp>

class OrientedBoundingBox;

/**
 * \brief View frustum defined by the six planes that bound it.
 */
class Frustum
{
   public:
    /**
     * \brief Extracts the planes from a view projection matrix.
     */
    explicit Frustum(const glm::mat4& viewProjection);

    /**
     * \brief Tests if a box is inside the frustum.
     */
    bool intersects(const OrientedBoundingBox& box) const;

   private:
    glm::vec4 m_planes[6];
};
