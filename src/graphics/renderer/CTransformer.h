#pragma once

#include <glm/glm.hpp>

/**
* \brief Transformer class calculates and stores all necessary transformations.
*/
class CTransformer
{
public:
	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);

	void setViewMatrix(const glm::mat4& view);
	void setProjectionMatrix(const glm::mat4& projection);

	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;
	const glm::vec3& getScale() const;

	const glm::mat4& getTranslationMatrix() const;
	const glm::mat4& getInverseTranslationMatrix() const;

	const glm::mat4& getRotationMatrix() const;
	const glm::mat4& getInverseRotationMatrix() const;
	
	const glm::mat4& getScaleMatrix() const;
	const glm::mat4& getInverseScaleMatrix() const;

	const glm::mat4& getModelMatrix() const;
	const glm::mat4& getInverseModelMatrix() const;

	const glm::mat4& getViewMatrix() const;
	const glm::mat4& getInverseViewMatrix() const;

	const glm::mat4& getProjectionMatrix() const;
	const glm::mat4& getInverseProjectionMatrix() const;

	const glm::mat4& getViewProjectionMatrix() const;
	const glm::mat4& getInverseViewProjectionMatrix() const;

	const glm::mat4& getModelViewProjectionMatrix() const;
	const glm::mat4& getInverseModelViewProjectionMatrix() const;

private:
	glm::vec3 m_positionVector = glm::vec3(0.f);
	glm::vec3 m_rotationVector = glm::vec3(0.f);
	glm::vec3 m_scaleVector = glm::vec3(1.f);

	struct STransfrom
	{
		bool m_matrixDirty = true;
		bool m_inverseDirty = true;
		glm::mat4 m_matrix = glm::mat4(1.f);
		glm::mat4 m_inverse = glm::mat4(1.f);
	};

	mutable STransfrom m_translation;
	mutable STransfrom m_rotation;
	mutable STransfrom m_scale;
	mutable STransfrom m_model;

	mutable STransfrom m_view;
	mutable STransfrom m_projection;
	mutable STransfrom m_viewProjection;

	mutable STransfrom m_modelViewProjection;
};