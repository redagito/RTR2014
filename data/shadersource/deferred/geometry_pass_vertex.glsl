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
	// Clip space position
	gl_Position = projection * view * model * vec4(vertexPositionModelSpace, 1.f);
	
	// Forward texture coordinates
	uv = vertexUV;
    
    // World space position; the cotangent frame for normal mapping derives its
    // tangent basis from screen-space derivatives of this value, so it must be
    // the true transformed position, not a rotation-only direction.
    vertexWorldSpace = vec3(model * vec4(vertexPositionModelSpace, 1.f));
    normalVectorWorldSpace = normalize(rotation * vec4(vertexNormalModelSpace, 0.f)).xyz;
}