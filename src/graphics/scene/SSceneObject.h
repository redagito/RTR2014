#pragma once

#include <glm/glm.hpp>

#include "graphics/ResourceConfig.h"

/**
* \brief Scene object data struct.
*/
struct SSceneObject
{
    SSceneObject();
    SSceneObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                 const glm::vec3& rotation, const glm::vec3& scale);

    ResourceId m_mesh;
    ResourceId m_material;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
};