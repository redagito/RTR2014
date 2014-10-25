#version 330 core

uniform vec4 color;

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = color;
}