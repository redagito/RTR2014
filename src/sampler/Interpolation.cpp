#include "Interpolation.h"

float QuadricInterpolation(float x0, float y0, float x1, float y1, float x2, float y2, float x)
{
	assert(x0 - x1 != 0.f);
	assert(x0 - x2 != 0.f);
	assert(x1 - x2 != 0.f);

	// Quadric interpolation constants
	float a = (x0 * (y2 - y1) + x1 * (y0 - y2) + x2 * (y1 - y0)) / ((x0 - x1) * (x0 - x2) * (x1 - x2));
	float b = (y1 - y0 + a * x0*x0 - a * x1*x1) / (x1 - x0);
	float c = y0 - a * x0*x0 - b * x0;

	// Caclulate value
	return a * x * x + b * x + c;
}