#include "CScreenQuadPass.h"

#include "graphics/renderer/core/RendererCoreConfig.h"

CScreenQuadPass::CScreenQuadPass(CShaderProgram* shader)
	:
	m_shader(shader)
{
	std::vector<float> vertices = {0.f, 0.f, 0.f};
	std::vector<unsigned int> indices = {1};
	std::vector<float> normals = { 0.f, 1.f, 0.f };
	std::vector<float> uvs = {0.f, 0.f, 0.f};
	m_quad.reset(new CMesh(vertices, indices, normals, uvs, EPrimitiveType::Point));
}

void CScreenQuadPass::draw(CTexture* texture, CFrameBuffer* fbo)
{
	if (fbo == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		fbo->setActive(GL_FRAMEBUFFER);
	}

	m_shader->setActive();
	texture->setActive(0);
	m_shader->setUniform("color_texture", 0);

	m_quad->getVertexArray()->setActive();
	glDrawArrays(GL_POINTS, 0, 1);
	m_quad->getVertexArray()->setInactive();
	m_shader->setInactive();
}