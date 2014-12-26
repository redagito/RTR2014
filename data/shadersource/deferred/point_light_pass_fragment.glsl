#version 330 core

layout(location = 0) out vec4 light_data;

uniform vec3 lightPositionWorldSpace;
uniform vec3 lightColor;
uniform float lightIntensity;

void main(void)
{
	light_data = vec4(lightColor * lightIntensity, 0.f);
}