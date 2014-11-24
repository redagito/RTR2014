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

// Filmic tonemapping operation based on "Uncharted 2" presentation 
// http://de.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting
// slide 140
// Includes gamma correction with gamma value 2.2f
vec3 filmicTonemapGamma(vec3 color)
{
	vec3 temp = max(vec3(0.f), color - 0.004);
	return (temp * (6.2f * temp + 0.5f)) / (temp * (6.2f * temp + 1.7f) + 0.06f);
}

// a = shoulder strength
// b = linear strength
// c = linear angle
// d = toe strength
// e = toe numerator
// f = toe denominator
vec3 filmicTonemapOperator(vec3 color, float a, float b, float c, float d, float e, float f)
{
	// e/f = toe angle
	return ((color * (a * color + b * c) + d * e) / (color * (a * color + b) + d * f)) - e / f;
}

// Filmic tonemap without gamma correction
vec3 filmicTonemap(vec3 color)
{
	float a = 0.22f;
	float b = 0.3f;
	float c = 0.1f;
	float d = 0.2f;
	float e = 0.01f;
	float f = 0.3f;
	
	// linearWhile = linear white point value
	vec3 linearWhite = vec3(11.2f);
	
	vec3 num = filmicTonemapOperator(color, a, b, c, d, e, f);
	vec3 denom = filmicTonemapOperator(linearWhite, a, b, c, d, e, f);
	return num / denom;
}

// Gamma correction
vec3 gammaCorrection(vec3 color)
{
	color.x = pow(color.x, 1.f / 2.2f);
	color.y = pow(color.y, 1.f / 2.2f);
	color.z = pow(color.z, 1.f / 2.2f);
	return color;
}

void main(void)
{
    
    vec3 worldPos = getWorldPosition();
    vec3 surfaceColor = texture(color_texture, uv).rgb;
    vec3 surfaceNormal = texture(normal_texture, uv).rgb * 2 - 1;
    
    fragmentColor = surfaceColor * 0.01;
        
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(    0, 0.5,  1), vec3(1.0, 0.2, 0.2), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -0.4, 0.5,  6), vec3(0.2, 1.0, 0.2), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(    0, 0.5, 11), vec3(0.2, 0.2, 1.0), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -0.6, 0.5, 16), vec3(0.5, 0.8, 0.8), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -2.0, 0.5, 21), vec3(1.0, 0.2, 0.2), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -4.3, 0.5, 26), vec3(0.5, 0.8, 0.8), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -9.2, 0.5, 31), vec3(1.0, 0.2, 0.2), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3(-15.0, 0.5, 36), vec3(0.2, 1.0, 0.2), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -3.7, 0.5, 36), vec3(0.2, 0.2, 1.0), 5.0);// * 0.5;
    fragmentColor = fragmentColor + illuminate(worldPos, surfaceColor, surfaceNormal, vec3( -3.6, 0.5, 41), vec3(0.5, 0.8, 0.8), 5.0);// * 0.5;
	
	// Tone mapping and gamma correction
	fragmentColor = gammaCorrection(filmicTonemap(fragmentColor));
	
}