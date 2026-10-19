#include "graphics/camera/CFirstPersonCamera.h"

#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

CFirstPersonCamera::CFirstPersonCamera(const glm::vec3& position, const glm::vec3& target,
                                       const glm::vec3& up, float fieldOfView, float aspectRatio,
                                       float zNear, float zFar)
{
    lookAt(position, target, up);
    setProjection(fieldOfView, aspectRatio, zNear, zFar);
}

glm::vec3 CFirstPersonCamera::getPosition() const { return m_position; }

void CFirstPersonCamera::setPosition(const glm::vec3& position)
{
    m_position = position;
    updateView();
}

void CFirstPersonCamera::move(const glm::vec3& direction)
{
    m_position += direction;
    updateView();
}

void CFirstPersonCamera::moveForward(float amount)
{
    m_position += glm::vec3(m_forward.x * amount, 0.0f, m_forward.y * amount);
    updateView();
}

void CFirstPersonCamera::moveRight(float amount)
{
    m_position += glm::vec3(m_forward.y * amount, 0.0f, -m_forward.x * amount);
    updateView();
}

void CFirstPersonCamera::moveUp(float amount)
{
    m_position += glm::vec3(0.0f, 1.0f, 0.0f) * amount;
    updateView();
}

void CFirstPersonCamera::rotate(const glm::vec3& axis, float amount)
{
    // TODO no support for arbitrary rotation ?
}

void CFirstPersonCamera::pitch(float amount)
{
    m_pitch += amount;
    m_pitch = fmin(m_pitch, glm::pi<float>() / 4);
    m_pitch = fmax(m_pitch, -glm::pi<float>() / 4);

    updateView();
}

void CFirstPersonCamera::roll(float amount)
{
    // TODO no support for rolling ?
}

void CFirstPersonCamera::yaw(float amount)
{
    glm::vec3 forward = TransformUtils::rotate(glm::vec3(0.0f, 1.0f, 0.0f), amount,
                                               glm::vec3(m_forward.x, 0.0f, m_forward.y));

    m_forward.x = forward.x;
    m_forward.y = forward.z;

    updateView();
}

void CFirstPersonCamera::lookAt(const glm::vec3& target, const glm::vec3& /* up */)
{
    glm::vec3 direction = target - m_position;
    if (glm::dot(direction, direction) < 1e-8f)
    {
        // Keep the current orientation when the target coincides with the eye.
        return;
    }

    glm::vec3 forward = glm::normalize(direction);
    m_pitch = -std::asin(glm::clamp(forward.y, -1.f, 1.f));

    glm::vec2 flat(forward.x, forward.z);
    if (glm::dot(flat, flat) < 1e-8f)
    {
        // Looking straight up or down leaves no horizontal heading to derive.
        flat = glm::vec2(0.f, 1.f);
    }
    m_forward = glm::normalize(flat);

    updateView();
}

const glm::mat4& CFirstPersonCamera::getView() const { return m_view; }

const glm::mat4& CFirstPersonCamera::getProjection() const { return m_proj; }

void CFirstPersonCamera::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
    m_position = eye;
    lookAt(target, up);
}

float CFirstPersonCamera::getFieldOfView() const { return m_fieldOfView; }

float CFirstPersonCamera::getZNear() const { return m_zNear; }

float CFirstPersonCamera::getZFar() const { return m_zFar; }

void CFirstPersonCamera::setProjection(float fieldOfView, float aspectRatio, float zNear,
                                       float zFar)
{
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_zNear = zNear;
    m_zFar = zFar;
    m_proj = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

void CFirstPersonCamera::updateView()
{
    glm::vec3 forward = glm::vec3(m_forward.x, 0.0f, m_forward.y);
    glm::vec3 axis = glm::vec3(m_forward.y, 0.0f, -m_forward.x);

    forward = TransformUtils::rotate(axis, m_pitch, forward);

    m_view = glm::lookAt(m_position, m_position + forward, glm::vec3(0.0, 1.0f, 0.0f));
}
