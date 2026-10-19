#pragma once

#include <vector>

#include <glm/glm.hpp>

/**
 * \brief Oriented bounding box.
 *
 * The box is defined by a center, three axes and the half extent along each axis. The axes are
 * orthonormal for a box computed from points. They are not necessarily orthonormal after a
 * transform with a non uniform scale.
 */
class OrientedBoundingBox
{
   public:
    OrientedBoundingBox();

    OrientedBoundingBox(const glm::vec3& center, const glm::mat3& axes,
                        const glm::vec3& halfExtents);

    /**
     * \brief Computes the box that contains a point cloud.
     *
     * The axes of the box are the principal components of the point cloud.
     */
    static OrientedBoundingBox fromPoints(const std::vector<glm::vec3>& points);

    /**
     * \brief Returns the box in the space of the given matrix.
     */
    OrientedBoundingBox transformed(const glm::mat4& matrix) const;

    /**
     * \brief Tests if the box is on the visible side of a plane.
     *
     * The plane is given as a normal and a distance, the normal points away from the visible side.
     */
    bool intersectsPlane(const glm::vec4& plane) const;

    const glm::vec3& getCenter() const { return m_center; }

    const glm::mat3& getAxes() const { return m_axes; }

    const glm::vec3& getHalfExtents() const { return m_halfExtents; }

   private:
    glm::vec3 m_center;
    glm::mat3 m_axes;
    glm::vec3 m_halfExtents;
};
