#include "ATransformable.h"

#include <glm/gtx/transform.hpp>

ATransformable::~ATransformable() {}

// data access

const glm::mat4& ATransformable::getModelMatrix() const { return m_modelMatrix; }

const glm::vec3& ATransformable::getPosition() const { return m_position; }

const glm::vec3& ATransformable::getUp() const { return m_up; }

const glm::vec3& ATransformable::getForward() const { return m_forward; }

const glm::vec3& ATransformable::getRight() const { return m_right; }

const float& ATransformable::getScale() const { return m_scale; }

// modificators

void ATransformable::moveInDirection(glm::vec3 direction)
{
    m_position += direction;
    updateModelMatrix();
}

void ATransformable::moveToPosition(glm::vec3 position)
{
    m_position = position;
    updateModelMatrix();
}

void ATransformable::moveUp(float amount)
{
    m_position += amount * m_up;
    updateModelMatrix();
}

void ATransformable::moveForward(float amount)
{
    m_position += amount * m_forward;
    updateModelMatrix();
}

void ATransformable::moveRight(float amount)
{
    // TODO
    // I experienced, that camera moves in the wrong direction, when calling this method.
    // So -= is used instead of += here.
    // This is either a problem of the camera or of the right-vector.
    // Investigate with scene objects!

    m_position -= amount * m_right;
    updateModelMatrix();
}

void ATransformable::roll(float amount)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1), amount, m_forward);

    glm::vec4 up4 = (rotation * glm::vec4(m_up.x, m_up.y, m_up.z, 1));
    m_up = glm::normalize(glm::vec3(up4.x, up4.y, up4.z));

    glm::vec4 right4 = (rotation * glm::vec4(m_right.x, m_right.y, m_right.z, 1));
    m_right = glm::normalize(glm::vec3(right4.x, right4.y, right4.z));

    updateModelMatrix();
}

void ATransformable::yaw(float amount)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1), amount, m_up);

    glm::vec4 right4 = (rotation * glm::vec4(m_right.x, m_right.y, m_right.z, 1));
    m_right = glm::normalize(glm::vec3(right4.x, right4.y, right4.z));

    glm::vec4 forward4 = (rotation * glm::vec4(m_forward.x, m_forward.y, m_forward.z, 1));
    m_forward = glm::normalize(glm::vec3(forward4.x, forward4.y, forward4.z));

    updateModelMatrix();
}

void ATransformable::pitch(float amount)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1), amount, m_right);

    glm::vec4 up4 = (rotation * glm::vec4(m_up.x, m_up.y, m_up.z, 1));
    m_up = glm::normalize(glm::vec3(up4.x, up4.y, up4.z));

    glm::vec4 forward4 = (rotation * glm::vec4(m_forward.x, m_forward.y, m_forward.z, 1));
    m_forward = glm::normalize(glm::vec3(forward4.x, forward4.y, forward4.z));

    updateModelMatrix();
}

void ATransformable::rotate(glm::vec3 axis, float amount)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1), amount, axis);

    glm::vec4 up4 = (rotation * glm::vec4(m_up.x, m_up.y, m_up.z, 1));
    m_up = glm::normalize(glm::vec3(up4.x, up4.y, up4.z));

    glm::vec4 right4 = (rotation * glm::vec4(m_right.x, m_right.y, m_right.z, 1));
    m_right = glm::normalize(glm::vec3(right4.x, right4.y, right4.z));

    glm::vec4 forward4 = (rotation * glm::vec4(m_forward.x, m_forward.y, m_forward.z, 1));
    m_forward = glm::normalize(glm::vec3(forward4.x, forward4.y, forward4.z));

    updateModelMatrix();
}

void ATransformable::scale(float amount)
{
    m_scale *= amount;
    updateModelMatrix();
}

void ATransformable::updateModelMatrix()
{
    glm::mat4 rotationMatrix =
        glm::mat4(-m_right.x, -m_right.y, -m_right.z, 0.0f, m_up.x, m_up.y, m_up.z, 0.0f,
                  m_forward.x, m_forward.y, m_forward.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    m_modelMatrix =
        glm::translate(m_position) * glm::scale(glm::mat4(1), glm::vec3(m_scale)) * rotationMatrix;
}
