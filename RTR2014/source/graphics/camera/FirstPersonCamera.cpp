#include "graphics/camera/FirstPersonCamera.h"

#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

FirstPersonCamera::FirstPersonCamera(const glm::vec3& position, const glm::vec3& target,
                                     float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    lookAt(position, target);
    setProjection(fieldOfView, aspectRatio, zNear, zFar);
}

glm::vec3 FirstPersonCamera::getPosition() const { return m_position; }

void FirstPersonCamera::setPosition(const glm::vec3& position)
{
    m_position = position;
    updateView();
}

void FirstPersonCamera::move(const glm::vec3& direction)
{
    m_position += direction;
    updateView();
}

void FirstPersonCamera::moveForward(float amount)
{
    m_position += glm::vec3(m_forward.x * amount, 0.0f, m_forward.y * amount);
    updateView();
}

void FirstPersonCamera::moveRight(float amount)
{
    m_position += glm::vec3(m_forward.y * amount, 0.0f, -m_forward.x * amount);
    updateView();
}

void FirstPersonCamera::moveUp(float amount)
{
    m_position += glm::vec3(0.0f, 1.0f, 0.0f) * amount;
    updateView();
}

void FirstPersonCamera::rotate(const glm::vec3& axis, float amount)
{
    // TODO no support for arbitrary rotation ?
}

void FirstPersonCamera::pitch(float amount)
{
    m_pitch += amount;
    m_pitch = fmin(m_pitch, glm::pi<float>() / 4);
    m_pitch = fmax(m_pitch, -glm::pi<float>() / 4);

    updateView();
}

void FirstPersonCamera::roll(float amount)
{
    // TODO no support for rolling ?
}

void FirstPersonCamera::yaw(float amount)
{
    glm::vec3 forward = TransformUtils::rotate(glm::vec3(0.0f, 1.0f, 0.0f), amount,
                                               glm::vec3(m_forward.x, 0.0f, m_forward.y));

    m_forward.x = forward.x;
    m_forward.y = forward.z;

    updateView();
}

void FirstPersonCamera::lookAt(const glm::vec3& target)
{
    glm::vec3 direction = target - m_position;
    if (glm::dot(direction, direction) < 1e-8f)
    {
        // Keep the current orientation when the target coincides with the eye.
        updateView();
        return;
    }

    glm::vec3 forward = glm::normalize(direction);
    // Clamp to the same range as pitch() so a vertical target cannot align
    // forward with the view up vector and degenerate glm::lookAt.
    m_pitch = -std::asin(glm::clamp(forward.y, -1.f, 1.f));
    m_pitch = fmin(m_pitch, glm::pi<float>() / 4);
    m_pitch = fmax(m_pitch, -glm::pi<float>() / 4);

    glm::vec2 flat(forward.x, forward.z);
    if (glm::dot(flat, flat) < 1e-8f)
    {
        // Looking straight up or down leaves no horizontal heading to derive.
        flat = glm::vec2(0.f, 1.f);
    }
    m_forward = glm::normalize(flat);

    updateView();
}

const glm::mat4& FirstPersonCamera::getView() const { return m_view; }

const glm::mat4& FirstPersonCamera::getProjection() const { return m_proj; }

void FirstPersonCamera::lookAt(const glm::vec3& eye, const glm::vec3& target)
{
    m_position = eye;
    lookAt(target);
}

float FirstPersonCamera::getFieldOfView() const { return m_fieldOfView; }

float FirstPersonCamera::getZNear() const { return m_zNear; }

float FirstPersonCamera::getZFar() const { return m_zFar; }

float FirstPersonCamera::getYawDegrees() const
{
    return glm::degrees(std::atan2(m_forward.x, m_forward.y));
}

float FirstPersonCamera::getPitchDegrees() const { return glm::degrees(-m_pitch); }

void FirstPersonCamera::setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_zNear = zNear;
    m_zFar = zFar;
    // The public API takes degrees, glm::perspective wants radians.
    m_proj = glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
}

void FirstPersonCamera::updateView()
{
    glm::vec3 forward = glm::vec3(m_forward.x, 0.0f, m_forward.y);
    glm::vec3 axis = glm::vec3(m_forward.y, 0.0f, -m_forward.x);

    forward = TransformUtils::rotate(axis, m_pitch, forward);

    m_view = glm::lookAt(m_position, m_position + forward, glm::vec3(0.0, 1.0f, 0.0f));
}
