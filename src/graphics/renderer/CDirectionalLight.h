#pragma once

#include "ALight.h"

class CDirectionalLight : public ALight
{
   private:
    glm::vec3 m_direction;
};
