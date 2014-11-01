#version 330 core

in vec2 uv;
uniform sampler2D color_texture;

#define T texture2D(color_texture, 0.5f + (p.xy *= 0.992f))

out vec4 color;

void main() 
{
	vec3 p = gl_FragCoord.xyz / vec3(800.f, 600.f, 1.f) - 0.5f;
	vec3 o = T.rbb;
	for (float i = 0.f; i < 100.f; ++i)
	{
		p.z += pow(max(0.f, 0.5f - length(T.rg)), 2.f) * exp(-i * 0.08f);
	}
	color = vec4(o * o + p.z, 1.f);
}