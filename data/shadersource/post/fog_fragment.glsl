#version 330 core

layout(location = 0) out vec4 output_color;

uniform sampler2D scene_texture;
uniform sampler2D depth_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

uniform int fog_type;

vec3 applyFog(vec3 color, float d)
{
    float n = 0.01f;
    float f = 1000.0f;
    float z = (2.0 * n * f) / (f + n - d * (f-n));
    
    float fogN = 0.0f;
    float fogF = 50.0f;
    float fogD = 0.02f;
    
    float fogFactor = 0;
    if (fog_type == 0) {
        return color;
    } else if (fog_type == 1) {
        fogFactor = (fogF - z) / (fogF - fogN);
    } else if (fog_type == 2) {
        fogFactor = exp(-fogD*z);
    } else if (fog_type == 3) {
        fogFactor = exp(-pow(fogD*z, 2.0f));
    }
    
    return mix(color, vec3(0.7, 0.6, 0.5), 1.0f - clamp(fogFactor, 0.5f, 1.0f));
}

void main(void)
{
    vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width,
                                              gl_FragCoord.y / screen_height);

    vec3 sceneColor = texture(scene_texture, normalized_screen_coordinates).xyz;
    
    float d = texture(depth_texture, normalized_screen_coordinates).x;
    
    output_color = vec4(applyFog(sceneColor, d), 1.0f);
}