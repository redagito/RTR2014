#include <spdlog/spdlog.h>
#include "graphics/ShaderProgram.h"


#include <cassert>

#include <glm/gtc/type_ptr.hpp>

GLuint ShaderProgram::s_activeShaderProgram = 0;

ShaderProgram::ShaderProgram(ShaderObject<GL_VERTEX_SHADER>* vertex,
                               ShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
                               ShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
                               ShaderObject<GL_GEOMETRY_SHADER>* geometry,
                               ShaderObject<GL_FRAGMENT_SHADER>* fragment)
    : m_programId(0), m_valid(false)
{
    init(vertex, tessControl, tessEval, geometry, fragment);
}

ShaderProgram::~ShaderProgram()
{
    if (m_valid)
    {
        if (s_activeShaderProgram == m_programId)
        {
            // The name is about to be reusable; the cache must not outlive it.
            s_activeShaderProgram = 0;
        }
        glDeleteProgram(m_programId);
    }
}

bool ShaderProgram::init(ShaderObject<GL_VERTEX_SHADER>* vertex,
                          ShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
                          ShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
                          ShaderObject<GL_GEOMETRY_SHADER>* geometry,
                          ShaderObject<GL_FRAGMENT_SHADER>* fragment)
{
    // Needs vertex shader
    if (vertex == nullptr || !vertex->isValid())
    {
        return false;
    }
    // Need fragment shader
    if (fragment == nullptr || !fragment->isValid())
    {
        return false;
    }

    // Check if any of the other shaders are invalid
    // nullptr signals unused stage
    if (geometry != nullptr && !geometry->isValid())
    {
        return false;
    }
    else if (tessControl != nullptr && !tessControl->isValid())
    {
        return false;
    }
    else if (tessEval != nullptr && !tessEval->isValid())
    {
        return false;
    }

    // Create program id
    GLuint programId = glCreateProgram();
    // Attach shader objects after checking validity

    // Needed stages
    glAttachShader(programId, vertex->getId());
    glAttachShader(programId, fragment->getId());
    // Optional stages
    if (geometry != nullptr)
    {
        glAttachShader(programId, geometry->getId());
    }
    if (tessControl != nullptr)
    {
        glAttachShader(programId, tessControl->getId());
    }
    if (tessEval != nullptr)
    {
        glAttachShader(programId, tessEval->getId());
    }

    // Link program
    glLinkProgram(programId);
    // Check result
    GLint result;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        // Set info log size
        GLint size = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &size);
        if (size > 0)
        {
            // Create buffer
            std::vector<GLchar> log(size + 1);
            // Get log
            glGetProgramInfoLog(programId, size, NULL, log.data());
            m_infoLog.assign(log.data(), size);
        }
        // Clean up temp id
        glDeleteProgram(programId);
        return false;
    }
    // New shader program linked successfully
    // Delete old shader program
    if (m_valid)
    {
        if (s_activeShaderProgram == m_programId)
        {
            s_activeShaderProgram = 0;
        }
        glDeleteProgram(m_programId);
    }
    // Set new id
    m_programId = programId;
    // Set validity flag
    m_valid = true;
    // Clear uniform location cache
    m_uniformLocations.clear();


    return true;
}

void ShaderProgram::setActive()
{
    if (!isValid())
    {
        spdlog::error("An invalid shader program cannot be used.");
        return;
    }
    if (s_activeShaderProgram != m_programId)
    {
        glUseProgram(m_programId);
        s_activeShaderProgram = m_programId;
    }
}

void ShaderProgram::setInactive()
{
    if (!isValid())
    {
        return;
    }
    if (s_activeShaderProgram == m_programId)
    {
        glUseProgram(0);
        s_activeShaderProgram = 0;
    }
}

const std::string& ShaderProgram::getErrorString() const { return m_infoLog; }

bool ShaderProgram::isValid() const { return m_valid; }

GLint ShaderProgram::getUniformLocation(const std::string& uniformName) const
{
    // Search for cached location
    auto iter = m_uniformLocations.find(uniformName);
    // Not found
    if (iter == m_uniformLocations.end())
    {
        // Cache location
        GLint location = glGetUniformLocation(m_programId, uniformName.data());
        m_uniformLocations[uniformName] = location;
        return location;
    }
    // Found
    return iter->second;
}

GLint ShaderProgram::getAttributeLocation(const std::string& attributeName) const
{
    return glGetAttribLocation(m_programId, attributeName.data());
}

void ShaderProgram::setUniform(GLint location, int i)
{
        glProgramUniform1i(m_programId, location, i);
}

void ShaderProgram::setUniform(const std::string& name, int i)
{
    setUniform(getUniformLocation(name), i);
}

void ShaderProgram::setUniform(GLint location, float f)
{
        glProgramUniform1f(m_programId, location, f);
}

void ShaderProgram::setUniform(const std::string& name, float f)
{
    setUniform(getUniformLocation(name), f);
}

void ShaderProgram::setUniform(GLint location, const glm::vec2& v)
{
	glProgramUniform2f(m_programId, location, v.x, v.y);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec2& v)
{
	setUniform(getUniformLocation(name), v);
}

void ShaderProgram::setUniform(GLint location, const glm::vec3& v)
{
        glProgramUniform3f(m_programId, location, v.x, v.y, v.z);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& v)
{
    setUniform(getUniformLocation(name), v);
}

void ShaderProgram::setUniform(GLint location, const glm::vec4& v)
{
        glProgramUniform4f(m_programId, location, v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& v)
{
    setUniform(getUniformLocation(name), v);
}

void ShaderProgram::setUniform(GLint location, const glm::mat2& m)
{
        glProgramUniformMatrix2fv(m_programId, location, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat2& m)
{
    setUniform(getUniformLocation(name), m);
}

void ShaderProgram::setUniform(GLint location, const glm::mat3& m)
{
        glProgramUniformMatrix3fv(m_programId, location, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& m)
{
    setUniform(getUniformLocation(name), m);
}

void ShaderProgram::setUniform(GLint location, const glm::mat4& m)
{
        glProgramUniformMatrix4fv(m_programId, location, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& m)
{
    setUniform(getUniformLocation(name), m);
}
