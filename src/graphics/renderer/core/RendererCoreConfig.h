#pragma once

#include <string>

#ifndef __APPLE__
#include "generated/flextGL.h"
#else
#define GLFW_INCLUDE_GLCOREARB
#define flextInit
#include "GLFW/glfw3.h"
#endif

/**
* Defines the default application-to-shader interface.
* TODO Cleanup?
*/

// Vertex array data locations
const GLuint vertexDataShaderLocation = 0;
const GLuint normalDataShaderLocation = 1;
const GLuint uvDataShaderLocation = 2;

// Screen parameters uniform names
const std::string screenWidthUniformName = "screen_width";
const std::string screenHeightUniformName = "screen_height";

// View and perspective matrix uniform names
const std::string viewMatrixUniformName = "view";
const std::string projectionMatrixUniformName = "projection";

// Transformation matrix uniform names
const std::string rotationMatrixUniformName = "rotation";
const std::string translationMatrixUniformName = "translation";
const std::string scaleMatrixUniformName = "scale";
const std::string modelMatrixUniformName = "model";

// Light parameter uniform names
const std::string lightPositionUniformName = "light_position";
const std::string lightRadiusUniformName = "light_radius";
const std::string lightIntensityUniformName = "light_intensity";
const std::string lightColorUniformName = "light_color";

// Texture units for geometry pass material textures
const GLint diffuseTextureUnit = 0;
const GLint normalTextureUnit = 1;
const GLint specularTextureUnit = 2;
const GLint glowTextureUnit = 3;
const GLint alphaTextureUnit = 4;

// Texture units for light pass
const GLint lightPassDepthTextureUnit = 0;
const GLint lightPassNormalSpecularTextureUnit = 1;

// Texture sampler uniform names
const std::string diffuseTextureUniformName = "diffuse_texture";
const std::string normalTextureUniformName = "normal_texture";
const std::string specularTextureUniformName = "specular_texture";
const std::string glowTextureUniformName = "glow_texture";
const std::string alphaTextureUniformName = "alpha_texture";
const std::string depthTextureUniformName = "depth_texture";
const std::string normalSpecularTextureUniformName = "normal_specular_texture";