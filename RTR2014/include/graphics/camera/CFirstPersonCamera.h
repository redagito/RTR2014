#pragma once

#include "glm/glm.hpp"

#include "IControllableCamera.h"
#include "TransformUtils.h"

class CFirstPersonCamera : public IControllableCamera
{
   public:
    CFirstPersonCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up,
                       float fieldOfView, float aspectRatio, float zNear, float zFar);

    const glm::mat4& getView() const override;
    const glm::mat4& getProjection() const override;

    glm::vec3 getPosition() const override;
    void setPosition(const glm::vec3& position) override;

    void move(const glm::vec3& direction) override;
    void moveForward(float amount) override;
    void moveRight(float amount) override;
    void moveUp(float amount) override;

    void rotate(const glm::vec3& axis, float amount) override;
    void pitch(float amount) override;
    void roll(float amount) override;
    void yaw(float amount) override;

    void lookAt(const glm::vec3& target, const glm::vec3& up) override;
    void lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) override;

    float getFieldOfView() const override;
    float getZNear() const override;
    float getZFar() const override;

    void setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar) override;

   private:
    void updateView();

    glm::mat4 m_view;
    glm::mat4 m_proj; /**< Projection matrix. */

    float m_fieldOfView; /**< Current field of view. */
    float m_aspectRatio; /**< Current aspect ratio. */
    float m_zNear;       /**< Current z near. */
    float m_zFar;        /**< Current z far. */

    glm::vec3 m_position;

    float m_pitch;
    glm::vec2 m_forward;
};