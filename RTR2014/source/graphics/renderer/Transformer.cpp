#include "graphics/renderer/Transformer.h"

#include <glm/ext/matrix_transform.hpp>

void Transformer::setPosition(const glm::vec3& position)
{
    m_positionVector = position;

    m_translation.m_matrixDirty = true;
    m_translation.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void Transformer::setRotation(const glm::vec3& rotation)
{
    m_rotationVector = rotation;

    m_rotation.m_matrixDirty = true;
    m_rotation.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void Transformer::setScale(const glm::vec3& scale)
{
    m_scaleVector = scale;

    m_scale.m_matrixDirty = true;
    m_scale.m_inverseDirty = true;

    m_model.m_matrixDirty = true;
    m_model.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void Transformer::setViewMatrix(const glm::mat4& view)
{
    m_view.m_matrix = view;

    m_view.m_matrixDirty = true;
    m_view.m_inverseDirty = true;

    m_viewProjection.m_matrixDirty = true;
    m_viewProjection.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

void Transformer::setProjectionMatrix(const glm::mat4& projection)
{
    m_projection.m_matrix = projection;

    m_projection.m_matrixDirty = true;
    m_projection.m_inverseDirty = true;

    m_viewProjection.m_matrixDirty = true;
    m_viewProjection.m_inverseDirty = true;

    m_modelViewProjection.m_matrixDirty = true;
    m_modelViewProjection.m_inverseDirty = true;
}

const glm::vec3& Transformer::getPosition() const { return m_positionVector; }

const glm::vec3& Transformer::getRotation() const { return m_rotationVector; }

const glm::vec3& Transformer::getScale() const { return m_scaleVector; }

const glm::mat4& Transformer::getTranslationMatrix() const
{
    if (m_translation.m_matrixDirty)
    {
        m_translation.m_matrix = glm::translate(glm::mat4(1.0f), m_positionVector);
        m_translation.m_matrixDirty = false;
    }
    return m_translation.m_matrix;
}

const glm::mat4& Transformer::getInverseTranslationMatrix() const
{
    if (m_translation.m_inverseDirty)
    {
        m_translation.m_inverse = glm::translate(glm::mat4(1.0f), -m_positionVector);
        m_translation.m_inverseDirty = false;
    }
    return m_translation.m_inverse;
}

const glm::mat4& Transformer::getRotationMatrix() const
{
    if (m_rotation.m_matrixDirty)
    {
        m_rotation.m_matrix = glm::rotate(glm::mat4(1.0f), m_rotationVector.x, glm::vec3(1.f, 0.f, 0.f)) *
                              glm::rotate(glm::mat4(1.0f), m_rotationVector.y, glm::vec3(0.f, 1.f, 0.f)) *
                              glm::rotate(glm::mat4(1.0f), m_rotationVector.z, glm::vec3(0.f, 0.f, 1.f));
        m_rotation.m_matrixDirty = false;
    }
    return m_rotation.m_matrix;
}

const glm::mat4& Transformer::getInverseRotationMatrix() const
{
    if (m_rotation.m_inverseDirty)
    {
        m_rotation.m_inverse = glm::rotate(glm::mat4(1.0f), -m_rotationVector.x, glm::vec3(1.f, 0.f, 0.f)) *
                               glm::rotate(glm::mat4(1.0f), -m_rotationVector.y, glm::vec3(0.f, 1.f, 0.f)) *
                               glm::rotate(glm::mat4(1.0f), -m_rotationVector.z, glm::vec3(0.f, 0.f, 1.f));
        m_rotation.m_inverseDirty = false;
    }
    return m_rotation.m_inverse;
}

const glm::mat4& Transformer::getScaleMatrix() const 
{
	if (m_scale.m_matrixDirty)
	{
		m_scale.m_matrix = glm::scale(glm::mat4(1.0f), m_scaleVector);
		m_scale.m_matrixDirty = false;
	}
	return m_scale.m_matrix;
}

const glm::mat4& Transformer::getInverseScaleMatrix() const 
{
	if (m_scale.m_inverseDirty)
	{
		// TODO Prevent div by zero?
		m_scale.m_inverse = glm::scale(glm::mat4(1.0f), 1.f / m_scaleVector);
		m_scale.m_inverseDirty = false;
	}
	return m_scale.m_inverse;
}

const glm::mat4& Transformer::getModelMatrix() const
{
	if (m_model.m_matrixDirty)
	{
		m_model.m_matrix = getTranslationMatrix() * getRotationMatrix() * getScaleMatrix();
		m_model.m_matrixDirty = false;
	}
	return m_model.m_matrix;
}

const glm::mat4& Transformer::getInverseModelMatrix() const 
{
	if (m_model.m_inverseDirty)
	{
		// Calculate with inverse matrices
		m_model.m_inverse = getInverseScaleMatrix() * getInverseRotationMatrix() * getInverseTranslationMatrix();
		m_model.m_inverseDirty = false;
	}
	return m_model.m_inverse;
}

const glm::mat4& Transformer::getViewMatrix() const
{
	// View matrix is set directly
	// TODO Set actual camera parameters and calculate view matrix on the fly
	return m_view.m_matrix;
}

const glm::mat4& Transformer::getInverseViewMatrix() const
{
	if (m_view.m_inverseDirty)
	{
		// Calculate with inverse matrix
		m_view.m_inverse = glm::inverse(m_view.m_matrix);
		m_view.m_inverseDirty = false;
	}
	return m_view.m_inverse;
}

const glm::mat4& Transformer::getProjectionMatrix() const
{
	// View projection matrix is set directly
	// TODO Set actual camera parameters and calculate projection matrix on the fly
	return m_projection.m_matrix;
}

const glm::mat4& Transformer::getInverseProjectionMatrix() const
{
	if (m_projection.m_inverseDirty)
	{
		// Calculate with inverse matrix
		m_projection.m_inverse = glm::inverse(m_projection.m_matrix);
		m_projection.m_inverseDirty = false;
	}
	return m_projection.m_inverse;
}

const glm::mat4& Transformer::getViewProjectionMatrix() const
{
	if (m_viewProjection.m_matrixDirty)
	{
		m_viewProjection.m_matrix = getProjectionMatrix() * getViewMatrix();
		m_viewProjection.m_matrixDirty = false;
	}
	return m_viewProjection.m_matrix;
}

const glm::mat4& Transformer::getInverseViewProjectionMatrix() const
{
	if (m_viewProjection.m_inverseDirty)
	{
		m_viewProjection.m_inverse = getInverseViewMatrix() * getInverseProjectionMatrix();
		m_viewProjection.m_inverseDirty = false;
	}
	return m_viewProjection.m_inverse;
}

const glm::mat4& Transformer::getModelViewProjectionMatrix() const
{
	if (m_modelViewProjection.m_matrixDirty)
	{
		m_modelViewProjection.m_matrix = getViewProjectionMatrix() * getModelMatrix();
		m_modelViewProjection.m_matrixDirty = false;
	}
	return m_modelViewProjection.m_matrix;
}

const glm::mat4& Transformer::getInverseModelViewProjectionMatrix() const
{
	if (m_modelViewProjection.m_inverseDirty)
	{
		m_modelViewProjection.m_inverse = getInverseModelMatrix() * getInverseViewProjectionMatrix();
		m_modelViewProjection.m_inverseDirty = false;
	}
	return m_modelViewProjection.m_inverse;
}
