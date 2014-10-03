#pragma once

#include <memory>

#include "core/RendererCoreConfig.h"

class CTexture;

/**
* \brief Represents a frame buffer (rendering target).
*/
class CFrameBuffer
{
public:
	CFrameBuffer();
	~CFrameBuffer();

	bool init();

	void setActive(GLenum target);
	void setInactive(GLenum target);

	void attach(const std::shared_ptr<CTexture>& texture, GLenum attachment);

private:
	GLuint m_fboId; /**< Frame buffer id. */
	bool m_valid; /**< Frame buffer validity. */
};