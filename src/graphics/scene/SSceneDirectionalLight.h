#pragma once

#include <glm/glm.hpp>

/**
* \brief Directional light object data.
*/
struct SSceneDirectionalLight
{
    SSceneDirectionalLight();
    SSceneDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

    glm::vec3 m_direction = glm::vec3(0.f, -1.f, 0.f); /**< Default direction is down 0/-1/0. */
    glm::vec3 m_color = glm::vec3(1.f);    /**< White color. */
    float m_intensity = 1.f;               /**< Light intensity. */
};