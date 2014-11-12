#pragma once

#include <memory>

#include "graphics/resource/CMesh.h"

class IResourceManager;
class CTexture;
class CFrameBuffer;
class IGraphicsResourceManager;

/**
* \brief Represents a draw pass to implement screen space effects.
* Performs a screen space effect with a texture input and frame buffer output.
*/
class CScreenSpacePass
{
public:
	/**
	 * \brief Performs mesh initialization.
	 */
	CScreenSpacePass();

	/**
	 * \brief Initialize resources.
	 */
	bool init(const std::string& shaderFile, IResourceManager* manager);

	/**
	* \brief Draws screen space quad with shader to render target and supplies textures as shader inputs.
	*/
	void draw(const IGraphicsResourceManager* manager, CFrameBuffer* fbo, CTexture* texture0 = nullptr,
			CTexture* texture1 = nullptr, CTexture* texture2 = nullptr, CTexture* texture3 = nullptr);

private:
	std::unique_ptr<CMesh> m_quad = nullptr; /**< Dummy mesh for fullscreen quad. */
	ResourceId m_shaderId = -1; /**< Shader program resource id. */
};
