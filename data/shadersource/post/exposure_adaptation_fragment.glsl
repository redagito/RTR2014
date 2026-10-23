#version 330 core

// The average logarithm of the luminance of the current frame
uniform sampler2D current_measurement;

// The adapted logarithm of the luminance of the previous frame
uniform sampler2D previous_exposure;

// How much of the current measurement replaces the history
uniform float adaptation_factor;

out float adapted_log_luminance;

void main(void)
{
	float level = log2(float(textureSize(current_measurement, 0).x));
	float current_log = textureLod(current_measurement, vec2(0.5), level).r;
	float previous_log = texture(previous_exposure, vec2(0.5)).r;
	adapted_log_luminance = mix(previous_log, current_log, adaptation_factor);
}
