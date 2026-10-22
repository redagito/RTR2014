#pragma once

#include <glm/glm.hpp>

/**
 * \brief Converts a color from sRGB to linear.
 *
 * Colors are authored as sRGB values, the calculations are done with linear
 * values. The textures are decoded by the hardware, which uses the exact sRGB
 * curve, this uses the gamma of 2.2 that the rest of the pipeline uses.
 */
inline glm::vec3 sRGBToLinear(const glm::vec3& color)
{
    return glm::pow(color, glm::vec3(2.2f));
}
