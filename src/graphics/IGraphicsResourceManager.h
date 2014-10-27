#pragma once

#include "resource/ResourceConfig.h"

class CMesh;
class CMaterial;
class CTexture;
class CShaderProgram;

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
	virtual CMesh* getMesh(ResourceId) const = 0;

	/**
	* \brief Maps id to internal material object.
	*/
	virtual CMaterial* getMaterial(ResourceId) const = 0;

	/**
	* \brief Maps id to internal texture object.
	*/
	virtual CTexture* getTexture(ResourceId) const = 0;

	/**
	* \brief Maps id to internal shader program object.
	*/
	virtual CShaderProgram* getShaderProgram(ResourceId) const = 0;

	/**
	* \brief Returns respective default texture.
	*/
	virtual CTexture* getDefaultDiffuseTexture() const = 0;
	virtual CTexture* getDefaultNormalTexture() const = 0;
	virtual CTexture* getDefaultSpecularTexture() const = 0;
	virtual CTexture* getDefaultGlowTexture() const = 0;
	virtual CTexture* getDefaultAlphaTexture() const = 0;
};