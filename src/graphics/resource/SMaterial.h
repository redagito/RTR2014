#pragma once

#include "graphics/ResourceConfig.h"

/**
 * \brief Material data.
 */
struct SMaterial
{
    SMaterial();
    SMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal, ResourceId specular,
              ResourceId glow, ResourceId customShader);
    ResourceId m_diffuse;
    ResourceId m_alpha;
    ResourceId m_normal;
    ResourceId m_specular;
    ResourceId m_glow;
	ResourceId m_customShader;
};
