#include "CFreeCamera.h"

#include <glm/ext.hpp>

CFreeCamera::CFreeCamera()
{
    // TODO replace with appropriate default values
    lookAt(glm::vec3(0.f, 7.f, -10.f), glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    setProjection(45.0f, 4.0f / 3.0f, 1.f, 1000.0f);
}

const glm::mat4& CFreeCamera::getView() const { return m_view; }

void CFreeCamera::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    m_position = eye;

    m_forward = glm::normalize(center - eye);
    m_right = glm::normalize(glm::cross(m_forward, up));
    m_up = glm::cross(m_right, m_forward);

    updateModelMatrix();
}

void CFreeCamera::setProjection(float fieldOfView, float aspectRatio, float zNear, float zFar)
{
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_zNear = zNear;
    m_zFar = zFar;
    m_proj = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

const glm::mat4& CFreeCamera::getProjection() const { return m_proj; }

float CFreeCamera::getAspectRatio() const { return m_aspectRatio; }

float CFreeCamera::getFieldOfView() const { return m_fieldOfView; }

float CFreeCamera::getZNear() const { return m_zNear; }

float CFreeCamera::getZFar() const { return m_zFar; }

void CFreeCamera::updateModelMatrix()
{
    ATransformable::updateModelMatrix();
    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}