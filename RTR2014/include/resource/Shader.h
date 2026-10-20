#pragma once

#include "resource/ResourceConfig.h"

struct Shader
{
    Shader();
    Shader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval, ResourceId geometry,
           ResourceId fragment);
    ResourceId m_vertex;
    ResourceId m_tessCtrl;
    ResourceId m_tessEval;
    ResourceId m_geometry;
    ResourceId m_fragment;
};
