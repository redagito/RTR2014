#pragma once

#include <glm/glm.hpp>

/**
 * \brief Camera interface class.
 */
class ICamera
{
   public:
    virtual ~ICamera();

    /**
    * \brief Returns view matrix.
    */
    virtual const glm::mat4& getView() const = 0;

    /**
    * \brief Returns projection matrix.
    */
    virtual const glm::mat4& getProjection() const = 0;
    
    virtual glm::vec3 getPosition() const = 0;
};
