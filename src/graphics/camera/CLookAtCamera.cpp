#include "CLookAtCamera.h"

#include <glm/ext.hpp>

CLookAtCamera::CLookAtCamera()
{
    // Default
    setView(glm::vec3(0.f, 7.f, -10.f), glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    setProjection(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

const glm::mat4& CLookAtCamera::getView() const { return m_view; }

void CLookAtCamera::setView(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    m_position = eye;
    m_view = glm::lookAt(eye, center, up);
}

void CLookAtCamera::setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    // Update member variables
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_zNear = zNear;
    m_zFar = zFar;
    m_proj = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

const glm::mat4& CLookAtCamera::getProjection() const { return m_proj; }

float CLookAtCamera::getAspectRatio() const { return m_aspectRatio; }

float CLookAtCamera::getFieldOfView() const { return m_fieldOfView; }

float CLookAtCamera::getZNear() const { return m_zNear; }

float CLookAtCamera::getZFar() const { return m_zFar; }

const glm::vec3& CLookAtCamera::getPosition() const { return m_position; }