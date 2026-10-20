#include "graphics/scene/SceneObject.h"

#include "graphics/renderer/Transformer.h"

SceneObject::SceneObject()
    : m_mesh(-1), m_material(-1), m_position(0.f), m_rotation(0.f), m_scale(1.f)
{
    return;
}

SceneObject::SceneObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                           const glm::vec3& rotation, const glm::vec3& scale,
                           const OrientedBoundingBox& localBounds, bool castsShadow)
    : m_mesh(mesh), m_material(material), m_position(position), m_rotation(rotation), m_scale(scale),
      m_localBounds(localBounds), m_castsShadow(castsShadow)
{
    updateWorldBounds();
    return;
}

void SceneObject::updateWorldBounds()
{
    Transformer transformer;
    transformer.setPosition(m_position);
    transformer.setRotation(m_rotation);
    transformer.setScale(m_scale);
    m_worldBounds = m_localBounds.transformed(transformer.getModelMatrix());
}
