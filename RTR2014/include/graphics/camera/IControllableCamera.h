#pragma once

#include "glm/glm.hpp"
#include "graphics/ICamera.h"

class IControllableCamera : public ICamera
{
   public:
    virtual ~IControllableCamera();

    /**
     * \brief Points the camera at a target.
     *
     * The camera has no roll, its up axis is the world up axis.
     */
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(const glm::vec3& position) = 0;

    virtual void move(const glm::vec3& direction) = 0;
    virtual void moveForward(float amount) = 0;
    virtual void moveRight(float amount) = 0;
    virtual void moveUp(float amount) = 0;

    virtual void rotate(const glm::vec3& axis, float amount) = 0;
    virtual void pitch(float amount) = 0;
    virtual void roll(float amount) = 0;
    virtual void yaw(float amount) = 0;

    virtual void lookAt(const glm::vec3& target) = 0;
    virtual void lookAt(const glm::vec3& position, const glm::vec3& target) = 0;

    virtual float getFieldOfView() const = 0;
    virtual float getZNear() const = 0;
    virtual float getZFar() const = 0;

    virtual void setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar) = 0;
};
