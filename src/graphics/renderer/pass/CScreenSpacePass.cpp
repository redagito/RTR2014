#include "CScreenSpacePass.h"

// Resource management
#include "resource/IResourceManager.h"

// Graphics API
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/resource/CTexture.h"
#include "graphics/resource/CShaderProgram.h"
#include "graphics/renderer/CFrameBuffer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"

// Debugging
#include "debug/Log.h"

CScreenSpacePass::CScreenSpacePass()
{
	std::vector<float> vertices = {0.f, 0.f, 0.f};
	std::vector<unsigned int> indices = {1};
	std::vector<float> normals = { 0.f, 1.f, 0.f };
	std::vector<float> uvs = {0.f, 0.f, 0.f};
	m_quad.reset(new CMesh(vertices, indices, normals, uvs, EPrimitiveType::Point));
}

bool CScreenSpacePass::init(const std::string& shaderFile, IResourceManager* manager)
{
	LOG_INFO("Initializing screen space pass with shader %s.", shaderFile.c_str());
	m_shaderId = manager->loadShader(shaderFile);
	if (m_shaderId == invalidResource)
	{
		LOG_ERROR("Failed to initialize the screen space pass from shader %s.", shaderFile.c_str());
		return false;
	}
	return true;
}

void CScreenSpacePass::draw(const IGraphicsResourceManager* manager, CFrameBuffer* fbo, CTexture* texture0,
		CTexture* texture1, CTexture* texture2, CTexture* texture3)
{
	if (m_shaderId == invalidResource)
	{
		LOG_ERROR("The screen space pass shader id is not valid.");
		return;
	}

	// Fetch shader based on id.
	// TODO Cache shader and only fetch on change (make listener)?
	CShaderProgram* shader = manager->getShaderProgram(m_shaderId);
	if (shader == nullptr)
	{
		LOG_ERROR("Failed to load screen space pass shader with id %.");
		return;
	}

	// Set rendering target
	if (fbo == nullptr)
	{
		// Default FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		// Use supplied FBO
		fbo->setActive(GL_FRAMEBUFFER);
	}

	// Screen space shader does not use depth testing
	// TODO Cache depth testing state.
	glDisable(GL_DEPTH_TEST);

	shader->setActive();
	// Set textures active if not null
	if (texture0 != nullptr)
	{
		texture0->setActive(0);
		shader->setUniform("texture0", 0);
	}
	if (texture1 != nullptr)
	{
		texture1->setActive(1);
		shader->setUniform("texture1", 1);
	}
	if (texture2 != nullptr)
	{
		texture2->setActive(2);
		shader->setUniform("texture2", 2);
	}
	if (texture3 != nullptr)
	{
		texture3->setActive(3);
		shader->setUniform("texture3", 3);
	}

	m_quad->getVertexArray()->setActive();
	glDrawArrays(GL_POINTS, 0, 1);
	m_quad->getVertexArray()->setInactive();
	shader->setInactive();

	glEnable(GL_DEPTH_TEST);
}
