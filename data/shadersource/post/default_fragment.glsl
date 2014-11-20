#version 330 core

in vec2 uv;

uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;

uniform mat4 inverse_view_proj;

out vec3 fragmentColor;

vec3 getWorldPosition() {
    float z = texture(depth_texture, uv).x;
    vec4 sPos = vec4(uv * 2 - 1, z * 2 - 1, 1.0);
    sPos = inverse_view_proj * sPos;
    return (sPos.xyz / sPos.w);
}

vec3 illuminate(vec3 worldPos, vec3 surfaceColor, vec3 surfaceNormal, vec3 lightPosition, vec3 lightColor, float intensity) {
    vec3 lightDirection = lightPosition - worldPos;
    float distance = length(lightDirection);
    lightDirection = lightDirection / distance;
    float attenuation = max(0.0, (intensity - distance) / intensity);
    return surfaceColor * max(0, dot(surfaceNormal, lightDirection)) * attenuation * lightColor;
}

void main(void)
{
    
    vec3 worldPos = getWorldPosition();
    vec3 surfaceColor = texture(color_texture, uv).rgb;
    vec3 surfaceNormal = texture(normal_texture, uv).rgb * 2 - 1;
    
    fragmentColor = surfaceColor * 0.1;
        
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(    0, 0.5,  1), vec3(1.0, 0.2, 0.2), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -0.4, 0.5,  6), vec3(0.2, 1.0, 0.2), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(    0, 0.5, 11), vec3(0.2, 0.2, 1.0), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -0.6, 0.5, 16), vec3(0.5, 0.8, 0.8), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -2.0, 0.5, 21), vec3(1.0, 0.2, 0.2), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -4.3, 0.5, 26), vec3(0.5, 0.8, 0.8), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -9.2, 0.5, 31), vec3(1.0, 0.2, 0.2), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(-15.0, 0.5, 36), vec3(0.2, 1.0, 0.2), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -3.7, 0.5, 36), vec3(0.2, 0.2, 1.0), 10.0) * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -3.6, 0.5, 41), vec3(0.5, 0.8, 0.8), 10.0) * 0.5;
    
    //fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(0,0,-25), vec3(1,1,1), 1000.0) * 0.9;
}