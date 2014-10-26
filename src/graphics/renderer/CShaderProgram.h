#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "core/RendererCoreConfig.h"

#include "TShaderObject.h"

/**
* \brief Represents a linked shader program.
*/
class CShaderProgram
{
   public:
    /**
    * \brief Creates shader program from shader objects.
    * Unused shader objects are represented by nullptr. All used shader objects
    * must be valid or program creation will fail and the program object will
    * be in invalid state after creation.
    */
    CShaderProgram(TShaderObject<GL_VERTEX_SHADER>* vertex,
                   TShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
                   TShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
                   TShaderObject<GL_GEOMETRY_SHADER>* geometry,
                   TShaderObject<GL_FRAGMENT_SHADER>* fragment);

    /**
    * \brief Deletes GPU shader resource.
    */
    ~CShaderProgram();

    /**
    * \brief Initializes the program with new shader objects.
    * \return True for success and false for error.
    * If the function returns false, an error string can be retrieved with
    * getErrorString.
    * Multiple calls to init reset the error string content.
    * If init is called on a valid shader program, a new program id is created
    * from the provided shader objects. If creation fails, the shader program will
    * still be valid with the data of the former initialization and an error string
    * can still be retrieved.
    */
    bool init(TShaderObject<GL_VERTEX_SHADER>* vertex,
              TShaderObject<GL_TESS_CONTROL_SHADER>* tessControl,
              TShaderObject<GL_TESS_EVALUATION_SHADER>* tessEval,
              TShaderObject<GL_GEOMETRY_SHADER>* geometry,
              TShaderObject<GL_FRAGMENT_SHADER>* fragment);

    /**
    * \brief Sets the shader program as active program for vertex processing.
    */
	void setActive();
	
	/**
	* \brief Sets shader program inactive.
	*/
	void setInactive();

    /**
    * \brief Returns the error string.
    * Set by a formerly failed call to init or on failed construction.
    */
    const std::string& getErrorString() const;

    /**
    * \brief Returns validity of the shader program.
    */
    bool isValid() const;

    GLint getUniformLocation(const std::string& uniformName) const;
    GLint getAttributeLocation(const std::string& attributeName) const;

    void setUniform(GLint location, int i);
    void setUniform(const std::string& uniformName, int i);

    void setUniform(GLint location, float f);
    void setUniform(const std::string& uniformName, float f);

    void setUniform(GLint location, const glm::vec2& v);
    void setUniform(const std::string& uniformName, const glm::vec2& v);

    void setUniform(GLint location, const glm::vec3& v);
    void setUniform(const std::string& uniformName, const glm::vec3& v);

    void setUniform(GLint location, const glm::vec4& v);
    void setUniform(const std::string& uniformName, const glm::vec4& v);

    void setUniform(GLint location, const glm::mat2& m);
    void setUniform(const std::string& uniformName, const glm::mat2& m);

    void setUniform(GLint location, const glm::mat3& m);
    void setUniform(const std::string& uniformName, const glm::mat3& m);

    void setUniform(GLint location, const glm::mat4& m);
    void setUniform(const std::string& uniformName, const glm::mat4& m);

   private:
    static GLuint s_activeShaderProgram; /**< Stores currently active shader id to prevent
                                                                             unnecessary calls to
                                            setActive. */
    mutable std::unordered_map<std::string, GLint>
        m_uniformLocations; /**< Caches uniform location ids. */
    std::string m_infoLog;
    GLuint m_programId;
    bool m_valid;
};
