#pragma once

#include <glm/glm.hpp>

class ATransformable
{
   public:
    virtual ~ATransformable() = 0;

    // data access

    virtual const glm::mat4& getModelMatrix() const;

    virtual const glm::vec3& getPosition() const;

    virtual const glm::vec3& getUp() const;
    virtual const glm::vec3& getForward() const;
    virtual const glm::vec3& getRight() const;

    virtual const float& getScale() const;

    // modificators

    virtual void moveInDirection(glm::vec3 direction);
    virtual void moveToPosition(glm::vec3 position);

    virtual void moveUp(float amount);
    virtual void moveForward(float amount);
    virtual void moveRight(float amount);

    virtual void roll(float amount);
    virtual void pitch(float amount);
    virtual void yaw(float amount);
    virtual void rotate(glm::vec3 axis, float amount);

    virtual void scale(float amount);

   protected:
    virtual void updateModelMatrix();
    
    glm::mat4 m_modelMatrix;

    glm::vec3 m_position = glm::vec3(0, 0, 0);

    glm::vec3 m_up = glm::vec3(0, 1, 0);
    glm::vec3 m_forward = glm::vec3(0, 0, 1);
    glm::vec3 m_right = glm::vec3(1, 0, 0);

    float m_scale = 1;
};