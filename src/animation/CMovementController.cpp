#include "CMovementController.h"

#include "graphics/IScene.h"

CMovementController::CMovementController(SceneObjectId id, AnimationObjectType type, IScene& scene,
                                         const glm::vec3& direction)
    : m_objectId(id), m_type(type), m_scene(scene), m_direction(direction)
{
    m_endTime = glm::length(m_direction) * 2;
}

void CMovementController::update(float timeStep)
{
    m_currentTimePoint += timeStep;
    if (m_currentTimePoint > m_endTime) {
        m_currentTimePoint -= m_endTime;
    }
    glm::vec3 diff = glm::normalize(m_direction) * timeStep;
    if (m_currentTimePoint > m_endTime / 2) {
        diff *= -1.0f;
    }
    
    if (m_type == AnimationObjectType::Model)
    {
        ResourceId mesh;
        ResourceId material;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        // Retrieve object
        if (!m_scene.getObject(m_objectId, mesh, material, position, rotation, scale))
        {
            // Invalid id?
            return;
        }
        position += diff;
        m_scene.setObject(m_objectId, mesh, material, position, rotation, scale);
    }
    else if (m_type == AnimationObjectType::PointLight)
    {
        glm::vec3 position;
        glm::vec3 color;
        float radius;
        float intensity;
		bool castsShadow;
        // Retrieve object
		if (!m_scene.getPointLight(m_objectId, position, radius, color, intensity, castsShadow))
        {
            // Invalid id?
            return;
        }
        position += diff;
		m_scene.setPointLight(m_objectId, position, radius, color, intensity, castsShadow);
    }
    else
    {
        // Error
    }
    return;
}