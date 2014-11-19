#version 330 core

in vec2 uv;

uniform sampler2D color_texture;
uniform sampler2D normal_texture;

out vec3 fragmentColor;

void main(void)
{
    vec3 lightDirection = normalize(vec3(0.0, 0.0, 1.0));
    
    vec3 surfaceColor = texture(color_texture, uv).rgb;
    vec3 surfaceNormal = texture(normal_texture, uv).rgb;
    
    fragmentColor = surfaceColor * 0.1 + surfaceColor * max(0, dot(surfaceNormal, lightDirection)) * 0.9;
}