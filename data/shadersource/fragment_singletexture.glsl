#version 330 core

in vec2 uv;

uniform sampler2D color_texture;

out vec3 fragmentColor;

void main(void)
{
	fragmentColor = texture(color_texture, uv).rgb;
	fragmentColor.r = 0.5f;
}