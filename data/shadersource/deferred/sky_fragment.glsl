#version 330 core

// The environment of the scene
uniform samplerCube sky_cubemap;

// The camera
uniform vec3 camera_position;
uniform mat4 inverse_view_projection;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Diffuse, glow
layout(location = 0) out vec4 diffuse_glow;
// Normal, specular
layout(location = 1) out vec4 normal_specular;

void main(void)
{
	vec2 normalized_screen_coordinates = gl_FragCoord.xy / vec2(screen_width, screen_height);
	vec2 ndc = normalized_screen_coordinates * 2.0 - 1.0;

	// The direction of the sky is the direction of the far plane, the inverse
	// view projection turns it back into the world.
	vec4 farPoint = inverse_view_projection * vec4(ndc, 1.0, 1.0);
	vec3 direction = normalize(farPoint.xyz / farPoint.w - camera_position);

	vec3 color = texture(sky_cubemap, direction).rgb;

	// The sky is the background, it is not lit by the scene and it does not
	// glow.
	diffuse_glow = vec4(color, 0.f);
	normal_specular = vec4(direction, 0.f);
}
