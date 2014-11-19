#version 330 core

in vec2 uv;

uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform sampler2D depth_texture;

uniform mat4 inverse_view_proj;

out vec3 fragmentColor;

vec3 getWorldPosition() {
    float n = 0.1f;
    float f = 1000.0f;
    
    float z = texture(depth_texture, uv).x;
    z = f * z + n - z * n;

    vec4 sPos = vec4(uv * 2.0 - 1.0, z, 1.0);
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
    
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(0, 0,  3), vec3(1,0,0), 10.0) * 0.45;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(0, 0, -3), vec3(0,1,0), 10.0) * 0.45;
    
    //fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(-3,-3,0), vec3(1,1,1), 100.0) * 0.9;
}