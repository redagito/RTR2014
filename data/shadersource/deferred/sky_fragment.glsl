#version 330 core

// The environment of the scene
uniform samplerCube sky_cubemap;

// The camera
uniform mat4 inverse_view;
uniform mat4 inverse_projection;

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

	// The direction of the sky is the direction of the view ray. The ray is
	// built in view space, the inverse projection accounts for the aspect
	// ratio. No division is needed, the w of the result is a constant, so the
	// derivatives of the direction stay accurate. A position on the far plane
	// would need a division by a varying w, the two terms of its derivative
	// cancel each other out and leave it without any precision.
	vec4 viewRay = inverse_projection * vec4(ndc, -1.0, 1.0);
	vec3 direction = normalize((inverse_view * vec4(viewRay.xyz, 0.0)).xyz);

	vec3 color = texture(sky_cubemap, direction).rgb;

	// The sky is the background, it is not lit by the scene and it does not
	// glow.
	diffuse_glow = vec4(color, 0.f);
	normal_specular = vec4(direction, 0.f);
}
