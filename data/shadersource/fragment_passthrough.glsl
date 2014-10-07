#version 330 core

in vec2 uv;
smooth in vec3 normalVectorCameraSpace;

out vec3 fragmentColor;

void main(void)
{
	fragmentColor = normalVectorCameraSpace;
}