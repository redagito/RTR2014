#pragma once

#include <glm/glm.hpp>

/**
 * \brief Abstract light class.
 */
class ALight
{
   public:
    virtual ~ALight();

   protected:
    ALight(const glm::vec3& color);

   private:
    glm::vec3 m_color; /**< Light color. */
};
