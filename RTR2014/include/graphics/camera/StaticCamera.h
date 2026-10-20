#pragma once

#include <glm/glm.hpp>

#include "graphics/ICamera.h"

/**
* \brief Camera with fixed view, projection and position matrices.
*/
class StaticCamera : public ICamera
{
   public:
    StaticCamera(const glm::mat4& view, const glm::mat4& proj, glm::vec3 position)
    {
        m_view = view;
        m_proj = proj;
        m_pos = position;
    }

    const glm::mat4& getView() const override { return m_view; }

    const glm::mat4& getProjection() const override { return m_proj; }

    glm::vec3 getPosition() const override { return m_pos; }

   private:
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::vec3 m_pos;
};