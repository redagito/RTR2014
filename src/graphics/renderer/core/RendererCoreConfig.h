#pragma once

#ifndef __APPLE__
#    include "generated/flextGL.h"
#else
#    define GLFW_INCLUDE_GLCOREARB
#    define flextInit
#    include "GLFW/glfw3.h"
#endif

// Vertex array data locations for shaders
const GLuint vertexDataShaderLocation = 0;
const GLuint normalDataShaderLocation = 1;
const GLuint uvDataShaderLocation = 2;