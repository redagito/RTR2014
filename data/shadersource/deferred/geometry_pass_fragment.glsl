#version 330 core

in vec2 uv;
smooth in vec3 normalVectorCameraSpace;
in vec3 normalVectorWorldSpace;

uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform sampler2D glow_texture;
uniform sampler2D alpha_texture;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;

void main(void)
{
	vec3 diffuseColor = texture(diffuse_texture, uv).rgb;
	vec3 textureNormal = texture(normal_texture, uv).rgb;
	
	float specular = texture(specular_texture, uv).r;
	float glow = texture(glow_texture, uv).r;
	float alpha = texture(alpha_texture, uv).r;

	diffuse.rgb = diffuseColor;
	diffuse.a = specular;
	
	//normal.rgb = textureNormal;
    normal.rgb = normalVectorWorldSpace;
	normal.a = glow;
}