#version 330 core

// http://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 uv;

void main() 
{
    gl_Position = vec4( 1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(-1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( 1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(-1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); 
}