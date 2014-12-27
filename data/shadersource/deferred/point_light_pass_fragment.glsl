#version 330 core

layout(location = 0) out vec4 light_data;

// Light parameters
uniform vec3 light_position;
uniform float light_radius;
uniform vec3 light_color;
uniform float light_intensity;

// Screen size
uniform float screen_width;
uniform float screen_height;

//
uniform mat4 inverse_view_projection;

// Depth, normal and specularity
uniform sampler2D depth_texture;
uniform sampler2D normal_specular_texture;

vec3 getWorldPosition(vec2 uv) 
{
    float z = texture(depth_texture, uv).x;
    vec4 sPos = vec4(uv * 2 - 1, z * 2 - 1, 1.0);
    sPos = inverse_view_projection * sPos;
    return (sPos.xyz / sPos.w);
}

void main(void)
{
	// Calculate screen position of the fragment [0-1]
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// Calculate world position of affected fragment
	vec3 fragment_world_position = getWorldPosition(normalized_screen_coordinates);
	// Temp storage for single texture fetch
	vec4 temp = texture(normal_specular_texture, normalized_screen_coordinates);
		
	// Store world space normal vector
	vec3 surface_normal_world = temp.xyz;
	// Specularity
	float specular = temp.w;
	
	float fragment_light_distance = distance(fragment_world_position, light_position);
	if (fragment_light_distance < light_radius)
	{
		float falloff = (light_radius - fragment_light_distance) / light_radius;
		
		//vec3 diffuse_light = dot()
		light_data = vec4(light_color * light_intensity * falloff, 0.f);
	}
	else
	{
		light_data = vec4(0.f);
	}
}