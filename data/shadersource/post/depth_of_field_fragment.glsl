#version 330 core

layout(location = 0) out vec4 output_color;

// Depth of field parameters
uniform float focus_near; // Closest non-blurry distance
uniform float blur_near; // Max blur at near distance

uniform float focus_far; // Furthest non-blurry distance
uniform float blur_far; // Max blur at far distance

// Screen size
uniform float screen_width;
uniform float screen_height;

// Transforms to camera coords
uniform mat4 inverse_projection;

// Depth
uniform sampler2D depth_texture;
uniform sampler2D base_texture;
uniform sampler2D blur_texture;

vec3 getCameraSpacePosition(vec2 uv)
{
	float z = texture(depth_texture, uv).x;
	vec4 sPos = vec4(uv * 2 - 1, z * 2 - 1, 1.0);
	sPos = inverse_projection * sPos;
	return (sPos.xyz / sPos.w);
}

void main(void)
{
	// Calculate screen position of the fragment [0-1]
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// Calculate world position of affected fragment
	vec3 fragment_camera_space_position = getCameraSpacePosition(normalized_screen_coordinates);

	float distance = length(fragment_camera_space_position);
	if (distance < focus_near)
	{
		float a = min(1.0, blur_near / distance);
		output_color = vec4(mix(texture(base_texture, normalized_screen_coordinates).xyz, texture(blur_texture, normalized_screen_coordinates).xyz, a), 0.0);
	}
	else if (distance > focus_far)
	{
		float a = min(1.0, distance / blur_far);
		output_color = vec4(mix(texture(base_texture, normalized_screen_coordinates).xyz, texture(blur_texture, normalized_screen_coordinates).xyz, a), 0.0);
	}
	else
	{
		output_color = vec4(texture(base_texture, normalized_screen_coordinates).xyz, 0.0);
	}
}