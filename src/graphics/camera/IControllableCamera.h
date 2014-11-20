#pragma once

#include "glm/glm.hpp"

#include "graphics/ICamera.h"

class IControllableCamera : public ICamera
{
   public:
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

    virtual void lookAt(const glm::vec3& target, const glm::vec3& up) = 0;
    virtual void lookAt(const glm::vec3& position, const glm::vec3& target,
                        const glm::vec3& up) = 0;

    virtual void setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar) = 0;
};