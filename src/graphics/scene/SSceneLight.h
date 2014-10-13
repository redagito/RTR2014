#pragma once

#include <glm/glm.hpp>

/**
* \brief Light object data.
* TODO Add constructors
*/
struct SSceneLight
{
    glm::vec3 m_position;
    float m_radius;
    glm::vec3 m_color;
};