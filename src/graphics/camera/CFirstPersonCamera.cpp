#include "CFirstPersonCamera.h"

const glm::mat4& CFirstPersonCamera::getView() const { return m_view; }

const glm::mat4& CFirstPersonCamera::getProjection() const { return m_projection; }