#include "resource/MaterialData.h"

MaterialData::MaterialData(ResourceId diffuse, ResourceId normal, ResourceId specular, ResourceId glow,
                     ResourceId alpha, ResourceId customShader)
    : m_diffuse(diffuse),
      m_normal(normal),
      m_specular(specular),
      m_glow(glow),
      m_alpha(alpha),
      m_customShader(customShader)
{
    return;
}

MaterialData::MaterialData()
    : m_diffuse(-1), m_normal(-1), m_specular(-1), m_glow(-1), m_alpha(-1), m_customShader(-1)
{
    return;
}
