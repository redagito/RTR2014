#version 330 core

// Vertex data streams
layout(location = 0) in vec3 vertex_position_normalized_screen_space;

void main(void)
{
	// Passthrough
	gl_Position = vec4(vertex_position_normalized_screen_space, 1.f);
}