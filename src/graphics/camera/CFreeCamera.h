#pragma once

#include "glm/glm.hpp"

#include "graphics/ATransformable.h"
#include "graphics/ICamera.h"

#include "input/IInputListener.h"

/**
* \brief Implements a look-at based camera.
*/
class CFreeCamera : public ICamera, public ATransformable
{
   public:
    /**
    * \brief Sets the view matrix.
    */
    CFreeCamera();

    /**
    * \brief Read access to the view matrix.
    */
    const glm::mat4& getView() const override;

    /**
    * \brief Returns projection matrix
    */
    const glm::mat4& getProjection() const override;

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
    * \brief Sets view matrix
    */
    void lookAt(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

    /**
    * \brief Sets projection matrix
    */
    void setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar);

   protected:
    void updateModelMatrix() override;

   private:
    glm::mat4 m_view;
    glm::mat4 m_proj; /**< Projection matrix. */

    float m_fieldOfView; /**< Current field of view. */
    float m_aspectRatio; /**< Current aspect ratio. */
    float m_zNear;       /**< Current z near. */
    float m_zFar;        /**< Current z far. */
};