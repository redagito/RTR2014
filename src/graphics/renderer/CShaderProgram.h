#pragma once

#include <glm/glm.hpp>

#include "core/RendererCoreConfig.h"

#include "TShaderObject.h"

/**
* \brief Represents a linked shader program.
*/
class CShaderProgram
{
   public:
    CShaderProgram(TShaderObject<GL_VERTEX_SHADER>* vertex,
                   TShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
                   TShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
                   TShaderObject<GL_GEOMETRY_SHADER>* geometry,
                   TShaderObject<GL_FRAGMENT_SHADER>* fragment);
    ~CShaderProgram();

    bool init(TShaderObject<GL_VERTEX_SHADER>* vertex,
              TShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
              TShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
              TShaderObject<GL_GEOMETRY_SHADER>* geometry,
              TShaderObject<GL_FRAGMENT_SHADER>* fragment);

    void setActive();

    const std::string& getErrorString() const;

    bool isValid() const;

    GLint getUniformLocation(const std::string& uniformName) const;
    GLint getAttributeLocation(const std::string& attributeName) const;

    void setUniform(GLint location, int i);
    void setUniform(GLint location, float f);

    void setUniform(GLint location, const glm::vec2& v);
    void setUniform(GLint location, const glm::vec3& v);
    void setUniform(GLint location, const glm::vec4& v);

    void setUniform(GLint location, const glm::mat2& m);
    void setUniform(GLint location, const glm::mat3& m);
    void setUniform(GLint location, const glm::mat4& m);

   private:
    static GLuint s_activeShaderProgram; /**< Stores currently active shader id to prevent
                                            unnecessary calls to setActive. */
    std::string m_infoLog;
    GLuint m_programId;
    bool m_valid;
};
