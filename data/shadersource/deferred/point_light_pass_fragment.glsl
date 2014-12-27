#version 330 core

layout(location = 0) out vec4 light_data;

uniform vec3 light_position;
uniform float light_radius;
uniform vec3 light_color;
uniform float light_intensity;

uniform sampler2D depth_texture;
uniform sampler2D normal_specular_texture;

uniform 

void main(void)
{
	light_data = vec4(light_color * light_intensity, 0.f);
}