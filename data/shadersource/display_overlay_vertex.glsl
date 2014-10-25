#version 330 core

layout(location = 0) in vec2 pixelCoords;

void main(void)
{
	// Output position of the vertex, in clip space
	// map [0..800][0..600] to [-1..1][-1..1]
	vec2 vertexPos = (pixelCoords - vec2(400,300)) / vec2(400,300);
	gl_Position = vec4(vertexPos, 0, 1);
}
