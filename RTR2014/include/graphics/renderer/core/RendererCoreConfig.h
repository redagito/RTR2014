#pragma once

#include <glad/gl.h>

#include <string>

/**
 * Defines the default application-to-shader interface.
 * TODO Cleanup?
 */

// Vertex array data locations
const GLuint vertexDataShaderLocation = 0;
const GLuint normalDataShaderLocation = 1;
const GLuint uvDataShaderLocation = 2;

// Screen parameters
const std::string screenWidthUniformName = "screen_width";
const std::string screenHeightUniformName = "screen_height";

// Camera parameters
const std::string cameraPositionUniformName = "camera_position";
const std::string cameraZNearUniformName = "camera_z_near";
const std::string cameraZFarUniformName = "camera_z_far";

// Depth-of-field parameters
const std::string focusNearUniformName = "focus_near";
const std::string focusFarUniformName = "focus_far";
const std::string blurNearUniformName = "blur_near";
const std::string blurFarUniformName = "blur_far";

// Blur parameters
const std::string blurStrengthUniformName = "blur_strength";

// Fog parameters
const std::string fogPassTypeUniformName = "fog_type";

// View and perspective matrix uniform names
const std::string viewMatrixUniformName = "view";
const std::string projectionMatrixUniformName = "projection";
const std::string inverseViewProjectionMatrixUniformName = "inverse_view_projection";
const std::string shadowViewProjectionBiasMatrixUniformName = "shadow_view_projection_bias";
const std::string shadowVolumeCenterUniformName = "shadow_volume_center";
const std::string shadowVolumeRadiusUniformName = "shadow_volume_radius";

// Transformation matrix uniform names
const std::string rotationMatrixUniformName = "rotation";
const std::string translationMatrixUniformName = "translation";
const std::string scaleMatrixUniformName = "scale";
const std::string modelMatrixUniformName = "model";
const std::string modelViewProjectionMatrixUniformName = "model_view_projection";

// Light parameter uniform names
const std::string lightPositionUniformName = "light_position";
const std::string lightDirectionUniformName = "light_direction";
const std::string lightRadiusUniformName = "light_radius";
const std::string lightIntensityUniformName = "light_intensity";
const std::string lightColorUniformName = "light_color";
const std::string lightCastsShadowUniformName = "casts_shadow";
const std::string normalMappingActiveUniformName = "normal_mapping_active";

// Texture units for geometry pass material textures
const GLint diffuseTextureUnit = 0;
const GLint normalTextureUnit = 1;
const GLint specularTextureUnit = 2;
const GLint glowTextureUnit = 3;
const GLint alphaTextureUnit = 4;

// Texture units for light pass
const GLint lightPassDepthTextureUnit = 0;
const GLint lightPassNormalSpecularTextureUnit = 1;
const GLint lightPassShadowMapTextureUnit = 2;  // Should be lightPassShadowTextureUnit

// Texture units for illumination pass
const GLint illuminationPassLightTextureUnit = 0;
const GLint illuminationPassDiffuseGlowTextureUnit = 1;

// Texture units for fog pass
const GLint fogPassSceneTextureUnit = 0;
const GLint fogPassDepthTextureUnit = 1;

// Texture units for display pass
const GLint displayPassSceneTextureUnit = 0;

// Texture units for FXAA pass
const GLint fxaaPassInputTextureUnit = 0;

// Texture units for vertical gauss blur pass
const GLint gaussBlurVerticalPassInputTextureUnit = 0;

// Texture units for horiontal gauss blur pass
const GLint gaussBlurHoriontalPassInputTextureUnit = 0;

// Texture units for depth-of-field pass
const GLint depthOfFieldPassSceneTextureUnit = 0;
const GLint depthOfFieldPassBlurTextureUnit = 1;
const GLint depthOfFieldPassDepthTextureUnit = 2;

// Texture units for depth texture visualization pass
const GLint visualizeDepthPassDepthTextureUnit = 0;

// Texture sampler uniform names
const std::string diffuseTextureUniformName = "diffuse_texture";
const std::string normalTextureUniformName = "normal_texture";
const std::string specularTextureUniformName = "specular_texture";
const std::string glowTextureUniformName = "glow_texture";
const std::string alphaTextureUniformName = "alpha_texture";
const std::string depthTextureUniformName = "depth_texture";
const std::string normalSpecularTextureUniformName = "normal_specular_texture";
const std::string diffuseGlowTextureUniformName = "diffuse_glow_texture";
const std::string lightTextureUniformName = "light_texture";
const std::string shadowMapTextureUniformName =
    "shadow_map";  // Should be shadow_map_texture or shadow_texture
const std::string shadowCubeTextureUniformName = "shadow_cube";
const std::string sceneTextureUniformName = "scene_texture";
const std::string blurTextureUniformName = "blur_texture";

// Generic texture names
const std::string texture0UniformName = "texture0";

// Generic texture units
const GLint texture0TextureUnit = 0;
const GLint texture1TextureUnit = 1;
const GLint texture2TextureUnit = 2;
const GLint texture3TextureUnit = 3;
const GLint texture4TextureUnit = 4;
