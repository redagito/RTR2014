#pragma once

#include <cassert>

/**
 * \brief Quadric interpolation for 3 samples.
 */
float quadricInterpolation(float x0, float y0, float x1, float y1, float x2, float y2, float x);