#include "SMaterial.h"

SMaterial::SMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal, ResourceId specular,
                     ResourceId glow)
    : m_diffuse(diffuse), m_alpha(alpha), m_normal(normal), m_specular(specular), m_glow(glow)
{
    return;
}

SMaterial::SMaterial() : m_diffuse(-1), m_alpha(-1), m_normal(-1), m_specular(-1), m_glow(-1)
{
    return;
}
