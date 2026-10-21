#version 330 core

// The screen quad is already in normalized device coordinates.
layout(location = 0) in vec3 vertex_position_normalized_screen_space;

void main(void)
{
	gl_Position = vec4(vertex_position_normalized_screen_space, 1.f);
}
