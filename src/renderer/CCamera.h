#pragma once

#include <glm/glm.hpp>

/**
 * \brief Represents a viewer of a scene.
 */
class CCamera
{

private:
	glm::mat4 m_view;
	glm::mat4 m_projection;
};
