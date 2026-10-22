#pragma once

#include <cmath>
#include <glm/glm.hpp>

/**
 * \brief Converts a color from sRGB to linear.
 *
 * Colors are authored as sRGB values, the calculations are done with linear
 * values. The textures are decoded by the hardware, which uses the exact sRGB
 * curve, this uses the gamma of 2.2 that the rest of the pipeline uses.
 */
inline float sRGBToLinear(float channel) { return std::pow(channel, 2.2f); }

inline glm::vec3 sRGBToLinear(const glm::vec3& color) { return glm::pow(color, glm::vec3(2.2f)); }

/**
 * \brief Converts a color from linear to sRGB.
 */
inline float linearToSRGB(float channel) { return std::pow(channel, 1.0f / 2.2f); }

inline glm::vec3 linearToSRGB(const glm::vec3& color)
{
    return glm::pow(color, glm::vec3(1.0f / 2.2f));
}
