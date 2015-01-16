#version 330 core

// Screen parameters
uniform float screen_width;
uniform float screen_height;

// Blur strength
// Set to 1 for normal blur
uniform float blur_strength;

// Input textur
uniform sampler2D texture0;

// Output to texture at location 0
layout(location = 0) out vec4 texture_blurred;

void main()
{
	// Calculate screen position of the fragment [0-1]
	vec2 screen_coords = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// Singel pixel size in 0-1 range
	float pixel_width = 1.0 / screen_width;
	float blur_width = pixel_width * blur_strength;

	// Horizontal gauss blur
	vec3 final_color = texture(texture0, vec2(screen_coords.x - blur_width * 3, screen_coords.y)).rgb * 0.006;
	final_color += texture(texture0, vec2(screen_coords.x - blur_width * 2, screen_coords.y)).rgb * 0.061;
	final_color += texture(texture0, vec2(screen_coords.x - blur_width, screen_coords.y)).rgb * 0.242;

	final_color += texture(texture0, vec2(screen_coords.x, screen_coords.y)).rgb * 0.383;

	final_color += texture(texture0, vec2(screen_coords.x + blur_width, screen_coords.y)).rgb * 0.242;
	final_color += texture(texture0, vec2(screen_coords.x + blur_width * 2, screen_coords.y)).rgb * 0.061;
	final_color += texture(texture0, vec2(screen_coords.x + blur_width * 3, screen_coords.y)).rgb * 0.006;

	texture_blurred = vec4(final_color, 1.f);
}