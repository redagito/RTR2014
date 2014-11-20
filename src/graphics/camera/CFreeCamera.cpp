#include "CFreeCamera.h"

#include <glm/ext.hpp>

CFreeCamera::CFreeCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up,
                         float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    lookAt(position, target, up);
    setProjection(fieldOfView, aspectRatio, zNear, zFar);
}

glm::vec3 CFreeCamera::getPosition() const { return m_position; }

void CFreeCamera::setPosition(const glm::vec3& position)
{
    m_position = position;
    updateView();
}

void CFreeCamera::move(const glm::vec3& direction)
{
    m_position += direction;
    updateView();
}

void CFreeCamera::moveForward(float amount)
{
    m_position += m_forward * amount;
    updateView();
}

void CFreeCamera::moveRight(float amount)
{
    m_position += m_right * amount;
    updateView();
}

void CFreeCamera::moveUp(float amount)
{
    m_position += m_up * amount;
    updateView();
}

void CFreeCamera::rotate(const glm::vec3& axis, float amount)
{
    m_up = TransformUtils::rotate(axis, amount, m_up);
    m_right = TransformUtils::rotate(axis, amount, m_right);
    m_forward = TransformUtils::rotate(axis, amount, m_forward);
    updateView();
}

void CFreeCamera::pitch(float amount)
{
    m_up = TransformUtils::rotate(m_right, amount, m_up);
    m_forward = TransformUtils::rotate(m_right, amount, m_forward);
    updateView();
}

void CFreeCamera::roll(float amount)
{
    m_up = TransformUtils::rotate(m_forward, amount, m_up);
    m_right = TransformUtils::rotate(m_forward, amount, m_right);
    updateView();
}

void CFreeCamera::yaw(float amount)
{
    m_right = TransformUtils::rotate(m_up, amount, m_right);
    m_forward = TransformUtils::rotate(m_up, amount, m_forward);
    updateView();
}

void CFreeCamera::lookAt(const glm::vec3& target, const glm::vec3& up)
{
    m_forward = glm::normalize(target - m_position);
    m_right = -glm::normalize(glm::cross(m_forward, up));
    m_up = glm::cross(m_forward, m_right);
    updateView();
}

const glm::mat4& CFreeCamera::getView() const { return m_view; }

const glm::mat4& CFreeCamera::getProjection() const { return m_proj; }

void CFreeCamera::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
    m_position = eye;
    lookAt(target, up);
}

void CFreeCamera::setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_zNear = zNear;
    m_zFar = zFar;
    m_proj = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

void CFreeCamera::updateView() { m_view = glm::lookAt(m_position, m_position + m_forward, m_up); }