#version 330 core

layout(location = 0) out vec4 output_color;

uniform sampler2D scene_texture;
uniform sampler2D depth_texture;

// Screen size
uniform float screen_width;
uniform float screen_height;

uniform int fog_type;

// The near and far plane of the camera, the depth is linearized with them
uniform float camera_z_near;
uniform float camera_z_far;

vec3 applyFog(vec3 color, float d)
{
    float n = camera_z_near;
    float f = camera_z_far;
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
    
    // The color of the fog is authored as sRGB, the scene is in linear space.
    vec3 fogColor = pow(vec3(0.7, 0.6, 0.5), vec3(2.2));

    return mix(color, fogColor, 1.0f - clamp(fogFactor, 0.0f, 1.0f));
}

void main(void)
{
    vec2 normalized_screen_coordinates = vec2(gl_FragCoord.x / screen_width,
                                              gl_FragCoord.y / screen_height);

    vec3 sceneColor = texture(scene_texture, normalized_screen_coordinates).xyz;

    float d = texture(depth_texture, normalized_screen_coordinates).x;

    // The sky is the background, it is not in the fog.
    if (d >= 1.0) {
        output_color = vec4(sceneColor, 1.0f);
        return;
    }

    output_color = vec4(applyFog(sceneColor, d), 1.0f);
}