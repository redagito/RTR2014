#version 330 core

// G-buffer textures
uniform sampler2D diffuse_glow_texture;
uniform sampler2D depth_texture;

// L-buffer texture
uniform sampler2D light_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Lit fragment
layout(location = 0) out vec3 fragmentColor;

// Filmic tonemapping operation based on "Uncharted 2" presentation 
// http://de.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting
// slide 140
// Includes gamma correction with gamma value 2.2f
vec3 filmicTonemapGamma(vec3 color)
{
	vec3 temp = max(vec3(0.f), color - 0.004);
	return (temp * (6.2f * temp + 0.5f)) / (temp * (6.2f * temp + 1.7f) + 0.06f);
}

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

	// linearWhile = linear white point value
	vec3 linearWhite = vec3(11.2f);

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
	// Retrieve gbuffer data
	vec4 temp = texture(diffuse_glow_texture, normalized_screen_coordinates);
	// Extract diffuse base color
	vec3 diffuseColor = temp.rgb;
	// Extract glow, should be used for blur pass
	vec3 glow = vec3(temp.a);
	// Retrieve light data
	vec3 light = texture(light_texture, normalized_screen_coordinates).rgb;
	
	// Perform tone mapping on incomming light
	// Should be done with average luminance
	light = filmicTonemap(light);

	// Calculate fragment color based on incomming light or glow
	fragmentColor = diffuseColor * max(light, glow);
}