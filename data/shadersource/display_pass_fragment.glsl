#version 330 core

// Final scene texture
uniform sampler2D scene_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Lit fragment
out vec3 color;

void main(void)
{
	// Get screen position for uv texture lookup
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// Write texture to screen
	color = texture(scene_texture, normalized_screen_coordinates).rgb;
}