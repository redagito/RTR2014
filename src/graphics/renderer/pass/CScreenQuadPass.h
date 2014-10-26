#pragma once

#include <memory>

#include "graphics/renderer/CTexture.h"
#include "graphics/renderer/CFrameBuffer.h"
#include "graphics/renderer/CMesh.h"
#include "graphics/renderer/CShaderProgram.h"

#include "resource/IResourceManager.h"

/**
* \brief Draws textured screen space quad.
*/
class CScreenQuadPass
{
public:
	/**
	* \brief Initializes default values.
	*/
	CScreenQuadPass(CShaderProgram* shader);

	/**
	* \brief Draws screen space quad with specified texture to specified frame buffer.
	*/
	void draw(CTexture* texture, CFrameBuffer* fbo = nullptr);

private:
	std::unique_ptr<CMesh> m_quad = nullptr;
	CShaderProgram* m_shader = nullptr;
};