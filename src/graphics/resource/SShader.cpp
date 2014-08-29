#include "SShader.h"

SShader::SShader() : m_vertex(-1), m_tessCtrl(-1), m_tessEval(-1), m_geometry(-1), m_fragment(-1)
{
    return;
}

SShader::SShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval, ResourceId geometry,
                 ResourceId fragment)
    : m_vertex(vertex),
      m_tessCtrl(tessCtrl),
      m_tessEval(tessEval),
      m_geometry(geometry),
      m_fragment(fragment)
{
    return;
}
