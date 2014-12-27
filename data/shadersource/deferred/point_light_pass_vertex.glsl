#version 330 core

// Vertex data streams
layout (location = 0) in vec3 vertex_position_model_space;

// Transformation matrices
uniform mat4 model_view_projection;

void main(void)
{
	// Calculate vertex position in camera space
	gl_Position = model_view_projection * vec4(vertex_position_model_space, 1.f);
}