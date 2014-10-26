#include "SMaterial.h"

SMaterial::SMaterial(ResourceId diffuse, ResourceId normal, ResourceId specular, ResourceId glow,
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

SMaterial::SMaterial() : m_diffuse(-1), m_normal(-1), m_specular(-1), m_glow(-1), m_alpha(-1)
{
    return;
}
