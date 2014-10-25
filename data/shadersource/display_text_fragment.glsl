#version 330 core

in vec2 ex_UV;

uniform sampler2D textureSampler;

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(textureSampler, ex_UV);
}