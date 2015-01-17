#version 330 core

uniform sampler2D scene_texture;
uniform float screen_width;
uniform float screen_height;

#define FXAA_REDUCE_MIN   (1.0/128.0)
#define FXAA_REDUCE_MUL   (1.0/8.0)
#define FXAA_SPAN_MAX     8.0

layout (location = 0) out vec3 color;

// Based on http://www.glge.org/demos/fxaa/
void main(){


	vec2 inverse_buffer_size = vec2(1.0 / screen_width, 1.0 / screen_height);

	vec3 rgbNW = texture(scene_texture, (gl_FragCoord.xy + vec2(-1.0, -1.0)) * inverse_buffer_size).xyz;
	vec3 rgbNE = texture(scene_texture, (gl_FragCoord.xy + vec2(1.0, -1.0)) * inverse_buffer_size).xyz;
	vec3 rgbSW = texture(scene_texture, (gl_FragCoord.xy + vec2(-1.0, 1.0)) * inverse_buffer_size).xyz;
	vec3 rgbSE = texture(scene_texture, (gl_FragCoord.xy + vec2(1.0, 1.0)) * inverse_buffer_size).xyz;
	vec3 rgbM = texture(scene_texture, gl_FragCoord.xy  * inverse_buffer_size).xyz;
	vec3 luma = vec3(0.299, 0.587, 0.114);

	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max(
		(lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
		FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
		dir * rcpDirMin)) * inverse_buffer_size;

	vec3 rgbA = 0.5 * (
		texture(scene_texture, gl_FragCoord.xy  * inverse_buffer_size + dir * (1.0 / 3.0 - 0.5)).xyz +
		texture(scene_texture, gl_FragCoord.xy  * inverse_buffer_size + dir * (2.0 / 3.0 - 0.5)).xyz);

	vec3 rgbB = rgbA * 0.5 + 0.25 * (
		texture(scene_texture, gl_FragCoord.xy  * inverse_buffer_size + dir *  -0.5).xyz +
		texture(scene_texture, gl_FragCoord.xy  * inverse_buffer_size + dir * 0.5).xyz);

	float lumaB = dot(rgbB, luma);
	
	if ((lumaB < lumaMin) || (lumaB > lumaMax)) 
	{
		color = rgbA;
	}
	else
	{
		color = rgbB;
	}
}