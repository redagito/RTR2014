#include "animation/RotationController.h"

#include <spdlog/spdlog.h>

#include <glm/trigonometric.hpp>

#include "graphics/IScene.h"

RotationController::RotationController(SceneObjectId id, AnimationObjectType type, IScene& scene, const glm::vec3& rotation)
	:
	m_objectId(id),
	m_type(type),
	m_scene(scene),
	m_rotation(rotation)
{

}

void RotationController::update(float timeStep)
{
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
		rotation += m_rotation * timeStep;
		// The angles are kept in a single turn, they would lose precision
		// otherwise.
		rotation = glm::mod(rotation, glm::vec3(2.f * 3.14159265358979323846f));
		m_scene.setObject(m_objectId, mesh, material, position, rotation, scale);
	}
	else if (m_type == AnimationObjectType::PointLight)
	{
		// Rotation point lights makes no sense
	}
	else
	{
		spdlog::error("Rotation controller does not support object type.");
	}
	return;
}
