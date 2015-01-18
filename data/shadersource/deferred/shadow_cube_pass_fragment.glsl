#version 330 core

// Material textures
uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform sampler2D glow_texture;

// Actually not needed here
uniform sampler2D alpha_texture;

uniform vec3 light_position;

in vec3 world_pos;

layout(location = 0) out float fragmentColor;

void main(void) {
    vec3 l2v = world_pos - light_position;
    float d = length(l2v);
    fragmentColor = d;
}