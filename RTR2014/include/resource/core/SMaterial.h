#pragma once

#include "resource/ResourceConfig.h"

/**
 * \brief Material data.
 */
struct SMaterial
{
    SMaterial();
    SMaterial(ResourceId diffuse, ResourceId normal, ResourceId specular, ResourceId glow,
              ResourceId alpha, ResourceId customShader);
    ResourceId m_diffuse;
    ResourceId m_normal;
    ResourceId m_specular;
    ResourceId m_glow;
    ResourceId m_alpha;
    ResourceId m_customShader;
};
