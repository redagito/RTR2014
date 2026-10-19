#include "graphics/scene/ScenePointLight.h"

ScenePointLight::ScenePointLight() {}

ScenePointLight::ScenePointLight(const glm::vec3& position, float radius, const glm::vec3& color,
                                   float intensity, bool castsShadow)
    : m_position(position),
      m_radius(radius),
      m_color(color),
      m_intensity(intensity),
      m_castsShadow(castsShadow)
{
}
