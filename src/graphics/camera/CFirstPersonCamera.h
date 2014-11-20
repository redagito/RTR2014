#pragma once

#include "glm/glm.hpp"

#include "IControllableCamera.h"
#include "TransformUtils.h"

class CFirstPersonCamera : public IControllableCamera
{
public:
    CFirstPersonCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up,
                float fieldOfView, float aspectRatio, float zNear, float zFar);
    
    virtual const glm::mat4& getView() const override;
    virtual const glm::mat4& getProjection() const override;
    
    virtual glm::vec3 getPosition() const override;
    virtual void setPosition(const glm::vec3& position) override;
    
    virtual void move(const glm::vec3& direction) override;
    virtual void moveForward(float amount) override;
    virtual void moveRight(float amount) override;
    virtual void moveUp(float amount) override;
    
    virtual void rotate(const glm::vec3& axis, float amount) override;
    virtual void pitch(float amount) override;
    virtual void roll(float amount) override;
    virtual void yaw(float amount) override;
    
    virtual void lookAt(const glm::vec3& target, const glm::vec3& up) override;
    virtual void lookAt(const glm::vec3& position, const glm::vec3& target,
                        const glm::vec3& up) override;
    
    virtual void setProjection(float fieldOfView, float aspectRatio, float zNear,
                               float zFar) override;
    
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