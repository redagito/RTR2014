#version 330 core

layout(location = 0) out vec4 output_color;

uniform sampler2D scene_texture;
uniform sampler2D depth_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

vec3 applyFog(vec3 color, float d)
{
    float n = 0.01f;
    float f = 1000.0f;
    float z = (2.0 * n * f) / (f + n - d * (f-n));
    
    float fogN = 0.0f;
    float fogF = 50.0f;
    float fogD = 0.02f;
    
    float fogFactor = exp(-pow(fogD*z, 2.0f));
    //float fogFactor = (fogF - z) / (fogF - fogN);
    
    return mix(color, vec3(0.7, 0.5, 0.3), 1.0f - clamp(fogFactor, 0.1f, 1.0f));
}

void main(void)
{
    vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width,
                                              gl_FragCoord.y / screen_height);

    vec3 sceneColor = texture(scene_texture, normalized_screen_coordinates).xyz;
    
    float d = texture(depth_texture, normalized_screen_coordinates).x;
    
    output_color = applyFog(sceneColor, d);
}