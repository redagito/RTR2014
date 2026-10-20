#include "animation/MovementController.h"

#include <spdlog/spdlog.h>

#include "graphics/IScene.h"

MovementController::MovementController(SceneObjectId id, AnimationObjectType type, IScene& scene,
                                       const glm::vec3& direction)
    : m_objectId(id), m_type(type), m_scene(scene), m_direction(direction)
{
    m_endTime = glm::length(m_direction) * 2;
}

float MovementController::offsetOfPhase(float phase) const
{
    return phase <= m_endTime / 2.0f ? phase : m_endTime - phase;
}

void MovementController::update(float timeStep)
{
    if (m_endTime <= 0.0f)
    {
        // Zero-length direction: nothing to animate, normalize would produce NaN.
        return;
    }

    const float previousPhase = m_currentTimePoint;
    m_currentTimePoint += timeStep;
    // The phase is within the cycle, a single subtraction is not enough if
    // timeStep exceeded the period.
    while (m_currentTimePoint >= m_endTime)
    {
        m_currentTimePoint -= m_endTime;
    }

    // The distance from the start of the cycle grows to the middle of it and
    // shrinks back afterwards. Deriving it from the phase instead of adding
    // up the steps keeps the object at its start position after a whole cycle.
    const glm::vec3 diff = glm::normalize(m_direction) *
                           (offsetOfPhase(m_currentTimePoint) - offsetOfPhase(previousPhase));

    if (m_type == AnimationObjectType::Model)
    {
        ResourceId mesh;
        ResourceId material;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        bool castsShadow = true;
        // Retrieve object
        if (!m_scene.getObject(m_objectId, mesh, material, position, rotation, scale, castsShadow))
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
        spdlog::error("Movement controller does not support object type.");
    }
    return;
}
