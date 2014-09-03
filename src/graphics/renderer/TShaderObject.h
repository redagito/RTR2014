#pragma once

#include <string>
#include <vector>

#include "core/RendererCoreConfig.h"

/**
* \brief Represents a compiled shader object.
*/
template<GLenum ShaderType>
class TShaderObject
{
public:
	/**
	* \brief Creates shader object from shader type and source code.
	*/
	TShaderObject(const std::string& source);
	~TShaderObject();

	/**
	* \brief Initializes shader with new source code.
	*/
	bool init(const std::string& source);

	/**
	* \brief Returns compile error string if a previous call to init returned false.
	*/
	const std::string& getErrorString() const;

	/**
	* \brief Returns shader object id.
	*/
	GLuint getId() const;

	/**
	* \brief Returns validity flag.
	*/
	bool isValid() const;

private:
	std::string m_infoLog;
	GLuint m_objectId;
	bool m_valid;
};

template<GLenum ShaderType>
TShaderObject<ShaderType>::TShaderObject(const std::string& source)
	:
	m_objectId(0),
	m_valid(false)
{
	init(source);
}

template<GLenum ShaderType>
TShaderObject<ShaderType>::~TShaderObject()
{
	glDeleteShader(m_objectId);
}

template<GLenum ShaderType>
bool TShaderObject<ShaderType>::init(const std::string& source)
{
	// Clean info log
	m_infoLog.clear();

	// Create new id
	GLuint objectId = glCreateShader(ShaderType);
	if (objectId == 0)
	{
		// Failed to create new id
		return false;
	}

	// Set source
	const GLchar* sourcePtr = source.data();
	glShaderSource(objectId, 1, &sourcePtr, NULL);

	// Compile
	glCompileShader(objectId);

	// Check error
	GLint result;
	glGetShaderiv(objectId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		// Set info log size
		GLint size;
		glGetShaderiv(objectId, GL_INFO_LOG_LENGTH, &size);
		if (size > 0)
		{
			// Create buffer
			std::vector<GLchar> log(size + 1);
			// Get log
			glGetShaderInfoLog(objectId, size, NULL, log.data());
			m_infoLog.assign(log.data(), size);
		}
		// Clean up temp id
		glDeleteShader(objectId);
		return false;
	}

	// New shader object compiled successfully
	// Delete old shader object
	if (m_valid)
	{
		glDeleteShader(m_objectId);
	}
	// Set new id
	m_objectId = objectId;
	// Set validity flag
	m_valid = true;
	return true;
}

template<GLenum ShaderType>
const std::string& TShaderObject<ShaderType>::getErrorString() const
{
	return m_infoLog;
}

template<GLenum ShaderType>
GLuint TShaderObject<ShaderType>::getId() const
{
	return m_objectId;
}

template<GLenum ShaderType>
bool TShaderObject<ShaderType>::isValid() const
{
	return m_valid;
}
