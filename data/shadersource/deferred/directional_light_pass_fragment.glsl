#version 330 core

layout(location = 0) out vec4 light_data;

// Light parameters
uniform vec3 light_direction;
uniform vec3 light_color;
uniform float light_intensity;

// Screen size
uniform float screen_width;
uniform float screen_height;

// Transforms to world coords
uniform mat4 inverse_view_projection;
uniform mat4 shadow_view_projection_bias;

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

mat3 cotangentFrame(vec3 N, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);

	// solve the linear system
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame 
	float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
	return mat3(T * invmax, B * invmax, N);
}

// Based on
// http://www.thetenthplanet.de/archives/1180
vec3 perturbNormal(vec3 normal, vec3 view_direction, vec2 uv)
{
	vec3 texture_normal = texture(normal_specular_texture, uv).xyz;
	texture_normal = texture_normal * 255.0 / 127.0 - 128.0 / 127.0;
	mat3 m = cotangentFrame(normal, -view_direction, uv);
	return normalize(m * texture_normal);
}

void main(void)
{
	// Calculate screen position of the fragment [0-1]
	vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);
	// Calculate world position of affected fragment
	vec3 fragment_world_position = getWorldPosition(normalized_screen_coordinates);
	
	// Hard coded world boundaries, hacky fix for skybox
	// TODO REMOVE THIIIIIIS IT HUUURTS
	if (fragment_world_position.x > 400.0 || fragment_world_position.x < -400.0 ||
		fragment_world_position.y > 400.0 || fragment_world_position.z < -400.0 ||
		fragment_world_position.z > 400.0 || fragment_world_position.z < -400.0)
	{
		light_data = vec4(2.0, 2.0, 2.0, 0.0);
		return;
	}

	// Temp storage for single texture fetch
	vec4 temp = texture(normal_specular_texture, normalized_screen_coordinates);

	// Store world space normal vector
	vec3 surface_normal_world = normalize(temp.xyz);

	// Apply normal mapping
	//surface_normal_world = perturbNormal(surface_normal_world, view_direction, normalized_screen_coordinates);

	// Specularity value
	float specular = temp.w;

	// Lambertian factor based on surface normal
	float lambert_factor = max(0.0, dot(surface_normal_world, -light_direction));
	float bias = 0.005f;

    // apply shadow map
    vec4 shadow_coordinates = shadow_view_projection_bias * vec4(fragment_world_position, 1);
    float visibility = texture(shadow_map, vec3(shadow_coordinates.xy, (shadow_coordinates.z - bias) / shadow_coordinates.w));

	// Calculate diffuse light contribution
	vec3 diffuse_light = lambert_factor * light_color * light_intensity;
    light_data = vec4(diffuse_light, 0.0) * visibility;
}