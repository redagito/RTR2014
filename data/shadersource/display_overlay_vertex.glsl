#version 330 core

layout(location = 0) in vec2 pixelCoords;

uniform vec2 resolution;

void main(void)
{
	// Output position of the vertex, in clip space
	// map [0..resolution.x][0..resolution.y] to [-1..1][-1..1]
	vec2 vertexPos = pixelCoords / resolution * 2.0 - 1.0;
	gl_Position = vec4(vertexPos, 0, 1);
}
