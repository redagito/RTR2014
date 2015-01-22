#include "SSceneDirectionalLight.h"

SSceneDirectionalLight::SSceneDirectionalLight() {}

SSceneDirectionalLight::SSceneDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                               float intensity, bool castsShadow)
    : m_direction(direction), m_color(color), m_intensity(intensity), m_castsShadow(castsShadow)
{
}