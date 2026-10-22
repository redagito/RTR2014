#version 330 core

// The radiance of the scene
uniform sampler2D scene_texture;

// The size of the metering texture, it covers the whole frame
uniform float metering_width;
uniform float metering_height;

// The logarithm of the luminance of the frame
out float luminance;

void main(void)
{
	// The center of the block of the frame this texel covers
	vec2 sampleUv = gl_FragCoord.xy / vec2(metering_width, metering_height);

	vec3 radiance = texture(scene_texture, sampleUv).rgb;
	// The mipmap chain averages the logarithm, its top level is the geometric
	// mean of the luminance of the frame
	luminance = log2(max(dot(radiance, vec3(0.2126, 0.7152, 0.0722)), 0.0001f));
}
