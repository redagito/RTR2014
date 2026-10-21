#version 330 core

uniform vec3 light_position;

in vec4 world_pos;

layout(location = 0) out float fragmentColor;

void main(void) {
    vec3 l2v = world_pos.xyz - light_position;
    float d = length(l2v);
    fragmentColor = d;
}