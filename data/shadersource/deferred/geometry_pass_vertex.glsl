#version 330 core

// Vertex data streams
layout (location = 0) in vec3 vertexPositionModelSpace;
layout (location = 1) in vec3 vertexNormalModelSpace;
layout (location = 2) in vec2 vertexUV;

// Transformation matrices
uniform mat4 model;
uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 scale;

// View and projection matrices
uniform mat4 view;
uniform mat4 projection;

// Texture coordinate
out vec2 uv;
// Nornal vector
smooth out vec3 normalVectorWorldSpace;

smooth out vec3 vertexWorldSpace;

void main(void)
{
	// Calculate vertex position in camera space
	gl_Position = projection * view * model * vec4(vertexPositionModelSpace, 1.f);
	
	// Forward texture coordinates
	uv = vertexUV;
    
    vertexWorldSpace = normalize(rotation * vec4(vertexPositionModelSpace, 0.f)).xyz;
    normalVectorWorldSpace = normalize(rotation * vec4(vertexNormalModelSpace, 0.f)).xyz;
}