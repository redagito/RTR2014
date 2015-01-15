#version 330 core

in vec2 uv;
smooth in vec3 normalVectorWorldSpace;

// Material textures
uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D specular_texture;
uniform sampler2D glow_texture;

// Actually not needed here
uniform sampler2D alpha_texture;

// Diffuse color and glow value
layout(location = 0) out vec4 diffuse_glow;
// Normals and specular value
layout(location = 1) out vec4 normal_specular;

void main(void)
{
    vec3 color = texture(diffuse_texture, uv).rgb;
    vec3 normal = texture(normal_texture, uv).rgb;
    
    float specular = texture(specular_texture, uv).r;
    float glow = texture(glow_texture, uv).r;
    //float alpha = texture(alpha_texture, uv).r;
    
    // Write diffuse map with glow
    diffuse_glow.rgb = color;
    diffuse_glow.a = glow;
    
    // normal.rgb = textureNormal;
    normal_specular.rgb = normalVectorWorldSpace;
    normal_specular.a = specular;
}