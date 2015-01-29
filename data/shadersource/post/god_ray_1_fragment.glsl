#version 330 core

// Light source position
uniform vec2 light_position_screen;

// Input scene texture
uniform sampler2D scene_texture;
uniform sampler2D depth_texture;

// Screen parameters
uniform float screen_width;
uniform float screen_height;

// Inverse projection matrix
uniform mat4 inverse_projection;

layout(location = 0) out vec3 color;

const float exposure = 0.6;
const float decay = 0.94;
const float density = 0.96;
const float weight = 0.2;
const int NUM_SAMPLES = 100;
const float clampMax = 1.0;

vec3 getCameraSpacePosition(vec2 uv)
{
	float z = texture(depth_texture, uv).x;
	vec4 sPos = vec4(uv * 2 - 1, z * 2 - 1, 1.0);
	sPos = inverse_projection * sPos;
	return (sPos.xyz / sPos.w);
}

// God ray pass
// References
// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch13.html
// http://fabiensanglard.net/lightScattering/
void main()
{
	vec2 uv = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);

	vec2 deltaTexCoord = uv - light_position_screen;
	deltaTexCoord *= 1.0 / float(NUM_SAMPLES) * density;

	// Initial sample
	color = texture(scene_texture, uv).xyz;
	if (color.x > 0.8 && color.y > 0.8 && color.z > 0.8)
	{
		color = vec3(1.0);
	}
	else
	{
		color = vec3(0.0);
	}

	vec2 coord = uv;
	
	float illuminationDecay = 1.0;


	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		// Step on ray
		coord -= deltaTexCoord;
		// Get sample
		vec3 texel = texture2D(scene_texture, coord).xyz;
		if (texel.x > 0.8 && texel.y > 0.8 && texel.z > 0.8)
		{
			texel = vec3(1.0);
		}
		else
		{
			texel = vec3(0.0);
		}

		texel *= illuminationDecay * weight;
		color += texel;

		illuminationDecay *= decay;
	}

	color *= exposure;
}