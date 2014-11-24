#pragma once

#include <glm/glm.hpp>

/**
* \brief Point light object data.
* TODO Add constructors
*/
struct SScenePointLight
{
    SScenePointLight();
    SScenePointLight(const glm::vec3& position, float radius, const glm::vec3& color);

    glm::vec3 m_position = glm::vec3(0.f); /**< Default position is origin at 0/0/0. */
    float m_radius = 1.f;                  /**< 1 unit light radius. */
    glm::vec3 m_color = glm::vec3(1.f);    /**< White color. */
};