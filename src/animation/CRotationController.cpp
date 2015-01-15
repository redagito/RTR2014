#include "CRotationController.h"

#include "graphics/IScene.h"

CRotationController::CRotationController(SceneObjectId id, AnimationObjectType type, IScene& scene, const glm::vec3& rotation)
	:
	m_objectId(id),
	m_type(type),
	m_scene(scene),
	m_rotation(rotation)
{

}

void CRotationController::update(float timeStep)
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
		m_scene.setObject(m_objectId, mesh, material, position, rotation, scale);
	}
	else if (m_type == AnimationObjectType::PointLight)
	{
		// Rotation point lights makes no sense
	}
	else
	{
		// Error
	}
	return;
}