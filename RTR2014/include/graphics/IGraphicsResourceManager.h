#pragma once

#include "resource/ResourceConfig.h"

class Mesh;
class Material;
class Texture;
class ShaderProgram;

/**
* \brief Graphics resource manager interface.
* Provides access to GPU resources.
*/
class IGraphicsResourceManager
{
public:
	/**
	* \brief Cleans up resource callbacks.
	*/
	virtual ~IGraphicsResourceManager();

	/**
	* \brief Maps id to internal mesh object.
	*/
	virtual Mesh* getMesh(ResourceId) const = 0;

	/**
	* \brief Maps id to internal material object.
	*/
	virtual Material* getMaterial(ResourceId) const = 0;

	/**
	* \brief Maps id to internal texture object.
	*/
	virtual Texture* getTexture(ResourceId) const = 0;

	/**
	* \brief Maps id to internal shader program object.
	*/
	virtual ShaderProgram* getShaderProgram(ResourceId) const = 0;

	/**
	* \brief Returns respective default texture.
	*/
	virtual Texture* getDefaultDiffuseTexture() const = 0;
	virtual Texture* getDefaultNormalTexture() const = 0;
	virtual Texture* getDefaultSpecularTexture() const = 0;
	virtual Texture* getDefaultGlowTexture() const = 0;
	virtual Texture* getDefaultAlphaTexture() const = 0;
};
