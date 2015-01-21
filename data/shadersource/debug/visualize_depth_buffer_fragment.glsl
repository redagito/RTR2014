#version 330 core

// Screen size
uniform float screen_width;
uniform float screen_height;

// Camera parameters
uniform float camera_z_near;
uniform float camera_z_far;

// Input textures
uniform sampler2D depth_texture;

// Output color
out vec3 color;

float getNormalizedDepth(vec2 uv)
{
	return texture(depth_texture, uv).x * 2 - 1.0;
}

void main(void)
{
	// Calculate screen position of the fragment [0-1]
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);

	// Based on http://www.geeks3d.com/20091216/geexlab-how-to-visualize-the-depth-buffer-in-glsl/
	float z_n = getNormalizedDepth(normalized_screen_coordinates);
	float z_linear = (2.0 * camera_z_near) / (camera_z_far + camera_z_near - z_n * (camera_z_far - camera_z_near));
	
	color = vec3(z_linear, z_linear, z_linear);
}