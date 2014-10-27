#pragma once

#include <memory>

#include "graphics/resource/CTexture.h"
#include "graphics/resource/CMesh.h"
#include "graphics/resource/CShaderProgram.h"
#include "graphics/renderer/CFrameBuffer.h"

#include "resource/IResourceManager.h"
#include "graphics/IGraphicsResourceManager.h"

/**
* \brief Draws textured screen space quad.
*/
class CScreenQuadPass
{
public:
	CScreenQuadPass();

	bool init(IResourceManager* manager);

	/**
	* \brief Draws screen space quad with specified texture to specified frame buffer.
	*/
	void draw(CTexture* texture, CFrameBuffer* fbo, const IGraphicsResourceManager* manager);

private:
	std::unique_ptr<CMesh> m_quad = nullptr;
	CShaderProgram* m_shader = nullptr;
	ResourceId m_shaderId = -1;
};