#include "CScreenQuadPass.h"

#include "graphics/renderer/core/RendererCoreConfig.h"

CScreenQuadPass::CScreenQuadPass()
{
	std::vector<float> vertices = {0.f, 0.f, 0.f};
	std::vector<unsigned int> indices = {1};
	std::vector<float> normals = { 0.f, 1.f, 0.f };
	std::vector<float> uvs = {0.f, 0.f, 0.f};
	m_quad.reset(new CMesh(vertices, indices, normals, uvs, EPrimitiveType::Point));
}

bool CScreenQuadPass::init(IResourceManager* manager)
{
	std::string screenQuadShaderFile = "data/shader/compose_screenquad.ini";
	m_shaderId = manager->loadShader(screenQuadShaderFile);
	if (m_shaderId == invalidResource)
	{
		LOG_ERROR("Failed to load screenquad shader %s.", screenQuadShaderFile.c_str());
		return false;
	}
	return true;
}

void CScreenQuadPass::draw(CTexture* texture, CFrameBuffer* fbo, const IGraphicsResourceManager* manager)
{
	m_shader = manager->getShaderProgram(m_shaderId);
	if (fbo == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		fbo->setActive(GL_FRAMEBUFFER);
	}

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shader->setActive();
	texture->setActive(0);
	m_shader->setUniform("color_texture", 0);

	m_quad->getVertexArray()->setActive();
	glDrawArrays(GL_POINTS, 0, 1);
	m_quad->getVertexArray()->setInactive();
	m_shader->setInactive();
}