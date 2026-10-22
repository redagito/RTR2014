#version 330 core

in vec2 uv;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D glow_texture;

out vec3 fragmentColor;

void main(void)
{
	vec3 diffuseColor = texture(diffuse_texture, uv).rgb;
	
	float specular = texture(specular_texture, uv).r;
	float glow = texture(glow_texture, uv).r;
	
	float ambient = 0.3f;
	
	// The radiance of the surface, the display pass maps it to the display
	// range.
	fragmentColor = diffuseColor * ambient + diffuseColor * glow + diffuseColor * specular;
	
}
