#version 330 core

// Final scene texture, it holds the linear radiance of the scene
uniform sampler2D scene_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Whether the radiance is mapped to the display range, the debug views show
// the raw data of the buffer they display
uniform int tonemap_enabled;

// Whether the measured brightness of the frame sets the exposure
uniform int exposure_enabled;

// The texture that holds the measurement of the brightness of the frame
uniform sampler2D exposure_texture;

// The key of the exposure, the luminance the average of the frame is mapped to
uniform float exposure_key;

// Lit fragment
out vec3 color;

// a = shoulder strength
// b = linear strength
// c = linear angle
// d = toe strength
// e = toe numerator
// f = toe denominator
vec3 filmicTonemapOperator(vec3 color, float a, float b, float c, float d, float e, float f)
{
	// e/f = toe angle
	return ((color * (a * color + b * c) + d * e) / (color * (a * color + b) + d * f)) - e / f;
}

// Filmic tonemap without gamma correction
vec3 filmicTonemap(vec3 color)
{
	float a = 0.22f;
	float b = 0.3f;
	float c = 0.1f;
	float d = 0.2f;
	float e = 0.01f;
	float f = 0.3f;

	// The linear white point, the radiance that is displayed as white.
	vec3 linearWhite = vec3(1.5f);

	vec3 num = filmicTonemapOperator(color, a, b, c, d, e, f);
	vec3 denom = filmicTonemapOperator(linearWhite, a, b, c, d, e, f);
	return num / denom;
}

// Gamma correction
vec3 gammaCorrection(vec3 color)
{
	color.x = pow(color.x, 1.f / 2.2f);
	color.y = pow(color.y, 1.f / 2.2f);
	color.z = pow(color.z, 1.f / 2.2f);
	return color;
}

void main(void)
{
	// Get screen position for uv texture lookup
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// The scene texture holds the linear radiance of the scene
	vec3 radiance = texture(scene_texture, normalized_screen_coordinates).rgb;

	if (tonemap_enabled != 0)
	{
		vec3 exposedRadiance = radiance;
		if (exposure_enabled != 0)
		{
			// The measurement is the average of the logarithm of the luminance
			// of the frame, the top level of the mipmap chain holds it.
			float level = log2(float(textureSize(exposure_texture, 0).x));
			float averageLuminance =
				exp2(textureLod(exposure_texture, vec2(0.5), level).r);

			// The key maps the average luminance of the frame to the middle
			// grey of the display range.
			exposedRadiance = radiance * (exposure_key / max(averageLuminance, 0.0001f));
		}

		// The radiance is mapped to the display range, the curve is applied to
		// the whole image, so that the sky and the scene have the same range.
		radiance = gammaCorrection(filmicTonemap(exposedRadiance));
	}

	// Write texture to screen
	color = radiance;
}
