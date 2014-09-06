#include "CVertexArrayObject.h"

CVertexArrayObject::CVertexArrayObject()
{
	glGenVertexArrays(1, &m_vaoId);
}

CVertexArrayObject::~CVertexArrayObject()
{
	glDeleteVertexArrays(1, &m_vaoId);
}

void CVertexArrayObject::setActive() const
{
	glBindVertexArray(m_vaoId);
	return;
}

void CVertexArrayObject::setInactive() const
{
	glBindVertexArray(0);
	return;
}

GLuint CVertexArrayObject::getId() const
{
	return m_vaoId;
}
