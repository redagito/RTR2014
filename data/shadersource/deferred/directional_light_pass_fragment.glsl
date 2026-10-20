#version 330 core

layout(location = 0) out vec4 light_data;

// Light parameters
uniform vec3 light_direction;
uniform vec3 light_color;
uniform float light_intensity;
uniform int casts_shadow;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Transforms to world coords
uniform mat4 inverse_view_projection;
uniform mat4 shadow_view_projection_bias;

// The sphere the shadow map covers
uniform vec3 shadow_volume_center;
uniform float shadow_volume_radius;

// Depth, normal and specularity
uniform sampler2D depth_texture;
uniform sampler2D normal_specular_texture;

// Shadow texture for shadow mapping
uniform sampler2DShadow shadow_map;

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
	
	// The environment around the scene is outside of the shadow volume, it is
	// lit without a shadow.
	if (distance(fragment_world_position, shadow_volume_center) > shadow_volume_radius) {
		light_data = vec4(2.0, 2.0, 2.0, 0.0);
		return;
	}

	// Temp storage for single texture fetch
	vec4 temp = texture(normal_specular_texture, normalized_screen_coordinates);

	// Store world space normal vector
	vec3 surface_normal_world = normalize(temp.xyz);

	// Specularity value
	float specular = temp.w;

	// Lambertian factor based on surface normal
	float lambert_factor = max(0.0, dot(surface_normal_world, -normalize(light_direction)));
	float bias = 0.005f;

    // apply shadow map
    float visibility = 1.0;
    if (casts_shadow != 0) {
        vec4 shadow_coordinates = shadow_view_projection_bias * vec4(fragment_world_position, 1);
        // Outside the shadow volume there is no occlusion information; skip the
        // lookup so the edge texel does not fake shadows.
        if (all(greaterThanEqual(shadow_coordinates.xy, vec2(0.0))) &&
            all(lessThanEqual(shadow_coordinates.xy, vec2(1.0))) &&
            shadow_coordinates.w > 0.0) {
            visibility = texture(shadow_map, vec3(shadow_coordinates.xy, (shadow_coordinates.z - bias) / shadow_coordinates.w));
        }
    }

	// Calculate diffuse light contribution
	vec3 diffuse_light = lambert_factor * light_color * light_intensity;
    light_data = vec4(diffuse_light, 0.0) * visibility;
}