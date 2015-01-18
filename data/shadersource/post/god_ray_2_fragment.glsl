#version 400

uniform sampler2D scene_texture;
uniform sampler2D godray_texture;

// Screen parameters
uniform float screen_width;
uniform float screen_height;

layout(location = 0) out vec3 color;

void main()
{
	vec2 uv = vec2(gl_FragCoord.x / screen_width, gl_FragCoord.y / screen_height);

	vec3 texel = texture(scene_texture, uv).xyz;
	vec3 texelGodrays = texture(godray_texture, uv).xyz;
	color = clamp(texel + texelGodrays, 0.0, 1.0);
}
