#include "SRenderRequest.h"

SRenderRequest::SRenderRequest()
    : m_mesh(nullptr), m_material(nullptr), m_translation(1.f), m_rotation(1.f), m_scale(1.f)
{
}

SRenderRequest::SRenderRequest(CMesh* mesh, CMaterial* material, const glm::mat4& translation,
                               const glm::mat4& rotation, const glm::mat4& scale)
    : m_mesh(mesh),
      m_material(material),
      m_translation(translation),
      m_rotation(rotation),
      m_scale(scale)
{
}