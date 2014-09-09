#pragma once

#include <glm/glm.hpp>

class CMesh;
class CMaterial;

struct SRenderRequest
{
	SRenderRequest();
	SRenderRequest(CMesh* mesh, CMaterial* material, const glm::mat4& translation, const glm::mat4& rotation, const glm::mat4& scale);

	CMesh* m_mesh;
	CMaterial* m_material;
	glm::mat4 m_translation;
	glm::mat4 m_rotation;
	glm::mat4 m_scale;
};