#include "SSceneObject.h"

SSceneObject::SSceneObject()
    : m_mesh(-1), m_material(-1), m_position(0.f), m_rotation(0.f), m_scale(1.f)
{
    return;
}

SSceneObject::SSceneObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                           const glm::vec3& rotation, const glm::vec3& scale)
    : m_mesh(mesh), m_material(material), m_position(position), m_rotation(rotation), m_scale(scale)
{
    return;
}