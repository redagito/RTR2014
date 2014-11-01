#include "CSampler.h"

CSampler::CSampler()
{
	glGenSamplers(1, &m_samplerId);
}

CSampler::~CSampler()
{
	glDeleteSamplers(1, &m_samplerId);
}

void CSampler::bind(GLuint unit)
{
	glBindSampler(unit, m_samplerId);
}

void CSampler::setMinFilter(GLint filter)
{
	glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, filter);
}

void CSampler::setMagFilter(GLint filter)
{
	glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, filter);
}