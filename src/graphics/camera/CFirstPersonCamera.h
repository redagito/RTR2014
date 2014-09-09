#pragma once

#include <glm/glm.hpp>

#include "graphics/ICamera.h"

/**
 * \brief First person camera class.
 * Provides camera manipulation for first person view.
 */
class CFirstPersonCamera : public ICamera
{
   public:
    const glm::mat4& getView() const;
    const glm::mat4& getProjection() const;

   private:
    glm::mat4 m_view;
    glm::mat4 m_projection;
};
