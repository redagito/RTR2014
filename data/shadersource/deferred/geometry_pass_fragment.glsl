#version 330 core

in vec2 uv;
smooth in vec3 normalVectorCameraSpace;

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform sampler2D glow_texture;
uniform sampler2D alpha_texture;

layout(location = 0) out vec3 diffuse;
layout(location = 1) out vec2 normal;
layout(location = 2) out vec2 glowSpecular;


void main(void)
{
	vec3 diffuseColor = texture(diffuse_texture, uv).rgb;
	vec3 textureNormal = texture(normal_texture, uv).rgb;
	
	float specular = texture(specular_texture, uv).r;
	float glow = texture(glow_texture, uv).r;
	float alpha = texture(alpha_texture, uv).r;
	
	float ambient = 0.3f;

	diffuse = diffuseColor;
	normal = textureNormal.xy;
	glowSpecular = vec2(glow, specular);
}