#pragma once

#include "glm/glm.hpp"

#include "graphics/ICamera.h"

/**
* \brief Implements a look-at based camera.
*/
class CLookAtCamera : public ICamera
{
   public:
    /**
    * \brief Sets the view matrix.
    */
    CLookAtCamera();

    /**
    * \brief Read access to the view matrix.
    */
    const glm::mat4& getView() const;

    /**
    * \brief Returns projection matrix
    */
    const glm::mat4& getProjection() const;

    /**
    * \brief Returns current aspect ratio
    */
    float getAspectRatio() const;

    /**
    * \brief Returns current field of view in degree
    */
    float getFieldOfView() const;

    /**
    * \brief Returns current z near
    */
    float getZNear() const;

    /**
    * \brief Returns current z far
    */
    float getZFar() const;

    /**
    * \brief Returns the camera position
    */
    const glm::vec3& getPosition() const;

    /**
    * \brief Sets view matrix
    */
    void setView(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

    /**
    * \brief Sets projection matrix
    */
    void setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar);

   private:
    glm::vec3 m_position; /**< Camera position. */
    glm::mat4 m_view;     /**< View matrix. */
    glm::mat4 m_proj;     /**< Projection matrix. */

    float m_fieldOfView; /**< Current field of view. */
    float m_aspectRatio; /**< Current aspect ratio. */
    float m_zNear;       /**< Current z near. */
    float m_zFar;        /**< Current z far. */
};