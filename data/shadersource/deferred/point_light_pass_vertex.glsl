#version 330 core

// Vertex data streams
layout (location = 0) in vec3 vertexPositionModelSpace;

// Transformation matrices
uniform mat4 model;

// View and projection matrices
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	// Calculate vertex position in camera space
	gl_Position = projection * view * model * vec4(vertexPositionModelSpace, 1.f);
}