#include "CTransformer.h"

#include <glm/ext.hpp>

void CTransformer::setPosition(const glm::vec3& position)
{
    m_positionVector = position;

    m_translation.m_matrixDirty = true;
    m_translation.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void CTransformer::setRotation(const glm::vec3& rotation)
{
    m_rotationVector = rotation;

    m_rotation.m_matrixDirty = true;
    m_rotation.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void CTransformer::setScale(const glm::vec3& scale)
{
    m_scaleVector = scale;

    m_scale.m_matrixDirty = true;
    m_scale.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void CTransformer::setViewMatrix(const glm::mat4& view)
{
    m_view.m_matrix = view;

    m_view.m_matrixDirty = true;
    m_view.m_inverseDirty = true;

    m_viewProjection.m_matrixDirty = true;
    m_viewProjection.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void CTransformer::setProjectionMatrix(const glm::mat4& projection)
{
    m_projection.m_matrix = projection;

    m_projection.m_matrixDirty = true;
    m_projection.m_inverseDirty = true;

    m_viewProjection.m_matrixDirty = true;
    m_viewProjection.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

const glm::vec3& CTransformer::getPosition() const { return m_positionVector; }

const glm::vec3& CTransformer::getRotation() const { return m_rotationVector; }

const glm::vec3& CTransformer::getScale() const { return m_scaleVector; }

const glm::mat4& CTransformer::getTranslationMatrix() const
{
    if (m_translation.m_matrixDirty)
    {
        m_translation.m_matrix = glm::translate(m_positionVector);
        m_translation.m_matrixDirty = false;
    }
    return m_translation.m_matrix;
}

const glm::mat4& CTransformer::getInverseTranslationMatrix() const
{
    if (m_translation.m_inverseDirty)
    {
        m_translation.m_inverse = glm::translate(-m_positionVector);
        m_translation.m_inverseDirty = false;
    }
    return m_translation.m_inverse;
}

const glm::mat4& CTransformer::getRotationMatrix() const
{
    if (m_rotation.m_matrixDirty)
    {
        m_rotation.m_matrix = glm::rotate(m_rotationVector.x, glm::vec3(1.f, 0.f, 0.f)) *
                              glm::rotate(m_rotationVector.y, glm::vec3(0.f, 1.f, 0.f)) *
                              glm::rotate(m_rotationVector.z, glm::vec3(0.f, 0.f, 1.f));
        m_rotation.m_matrixDirty = false;
    }
    return m_rotation.m_matrix;
}

const glm::mat4& CTransformer::getInverseRotationMatrix() const
{
    if (m_rotation.m_inverseDirty)
    {
        m_rotation.m_inverse = glm::rotate(-m_rotationVector.x, glm::vec3(1.f, 0.f, 0.f)) *
                               glm::rotate(-m_rotationVector.y, glm::vec3(0.f, 1.f, 0.f)) *
                               glm::rotate(-m_rotationVector.z, glm::vec3(0.f, 0.f, 1.f));
        m_rotation.m_inverseDirty = false;
    }
    return m_rotation.m_inverse;
}

const glm::mat4& CTransformer::getScaleMatrix() const 
{
	if (m_scale.m_matrixDirty)
	{
		m_scale.m_matrix = glm::scale(m_scaleVector);
		m_scale.m_matrixDirty = false;
	}
	return m_scale.m_matrix;
}

const glm::mat4& CTransformer::getInverseScaleMatrix() const 
{
	if (m_scale.m_inverseDirty)
	{
		// TODO Prevent div by zero?
		m_scale.m_inverse = glm::scale(1.f / m_scaleVector);
		m_scale.m_inverseDirty = false;
	}
	return m_scale.m_inverse;
}

const glm::mat4& CTransformer::getModelMatrix() const
{
	if (m_model.m_matrixDirty)
	{
		m_model.m_matrix = getTranslationMatrix() * getRotationMatrix() * getScaleMatrix();
		m_model.m_matrixDirty = false;
	}
	return m_model.m_matrix;
}

const glm::mat4& CTransformer::getInverseModelMatrix() const 
{
	if (m_model.m_inverseDirty)
	{
		// Calculate with inverse matrices
		m_model.m_inverse = getInverseScaleMatrix() * getInverseRotationMatrix() * getInverseTranslationMatrix();
		m_model.m_inverseDirty = false;
	}
	return m_model.m_inverse;
}

const glm::mat4& CTransformer::getViewMatrix() const
{
	// View matrix is set directly
	// TODO Set actual camera parameters and calculate view matrix on the fly
	return m_view.m_matrix;
}

const glm::mat4& CTransformer::getInverseViewMatrix() const
{
	if (m_view.m_inverseDirty)
	{
		// Calculate with inverse matrix
		m_view.m_inverse = glm::inverse(m_view.m_matrix);
		m_view.m_inverseDirty = false;
	}
	return m_view.m_inverse;
}

const glm::mat4& CTransformer::getProjectionMatrix() const
{
	// View projection matrix is set directly
	// TODO Set actual camera parameters and calculate projection matrix on the fly
	return m_projection.m_matrix;
}

const glm::mat4& CTransformer::getInverseProjectionMatrix() const
{
	if (m_projection.m_inverseDirty)
	{
		// Calculate with inverse matrix
		m_projection.m_inverse = glm::inverse(m_projection.m_matrix);
		m_projection.m_inverseDirty = false;
	}
	return m_projection.m_inverse;
}

const glm::mat4& CTransformer::getViewProjectionMatrix() const
{
	if (m_viewProjection.m_matrixDirty)
	{
		m_viewProjection.m_matrix = getProjectionMatrix() * getViewMatrix();
		m_viewProjection.m_matrixDirty = false;
	}
	return m_viewProjection.m_matrix;
}

const glm::mat4& CTransformer::getInverseViewProjectionMatrix() const
{
	if (m_viewProjection.m_inverseDirty)
	{
		m_viewProjection.m_inverse = getInverseViewMatrix() * getInverseProjectionMatrix();
		m_viewProjection.m_inverseDirty = false;
	}
	return m_viewProjection.m_inverse;
}

const glm::mat4& CTransformer::getModelViewProjectionMatrix() const
{
	if (m_modelViewProjection.m_matrixDirty)
	{
		m_modelViewProjection.m_matrix = getViewProjectionMatrix() * getModelMatrix();
		m_modelViewProjection.m_matrixDirty = false;
	}
	return m_modelViewProjection.m_matrix;
}

const glm::mat4& CTransformer::getInverseModelViewProjectionMatrix() const
{
	if (m_modelViewProjection.m_inverseDirty)
	{
		m_modelViewProjection.m_inverse = getInverseModelMatrix() * getInverseViewProjectionMatrix();
		m_modelViewProjection.m_inverseDirty = false;
	}
	return m_modelViewProjection.m_inverse;
}