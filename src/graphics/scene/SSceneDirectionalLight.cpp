#include "SSceneDirectionalLight.h"

SSceneDirectionalLight::SSceneDirectionalLight() {}

SSceneDirectionalLight::SSceneDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                               float intensity)
    : m_direction(direction), m_color(color), m_intensity(intensity)
{
}