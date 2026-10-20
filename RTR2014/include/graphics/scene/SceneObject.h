#pragma once

#include <glm/glm.hpp>

#include "resource/OrientedBoundingBox.h"
#include "resource/ResourceConfig.h"

/**
* \brief Scene object data struct.
*/
struct SceneObject
{
    SceneObject();
    SceneObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                 const glm::vec3& rotation, const glm::vec3& scale,
                 const OrientedBoundingBox& localBounds, bool castsShadow);

    /**
     * \brief Updates the world space bounds from the object transform.
     */
    void updateWorldBounds();

    ResourceId m_mesh;
    ResourceId m_material;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    OrientedBoundingBox m_localBounds;  /**< Bounds of the mesh in mesh space. */
    OrientedBoundingBox m_worldBounds;  /**< Bounds of the object in world space. */
    bool m_castsShadow = true;          /**< Whether the object casts a shadow. */
};
