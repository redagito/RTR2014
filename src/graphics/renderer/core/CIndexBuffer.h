#pragma once

#include <vector>

#include "RendererCoreConfig.h"

/**
* \brief Represents an index buffer.
*/
class CIndexBuffer
{
   public:
    CIndexBuffer(const std::vector<unsigned int>& indices, GLenum usage = GL_STATIC_DRAW);

    CIndexBuffer(const CIndexBuffer& rhs) = delete;

    ~CIndexBuffer();

    CIndexBuffer& operator=(const CIndexBuffer& rhs) = delete;

    /**
    * \brief Sets the VBO as active object.
    * Binds the VBO to GL_INDEX_ARRAY.
    */
    void setActive() const;
	void setInactive() const;

    /**
    * \brief Returns buffer validity.
    */
    bool isValid() const;

    /**
    * \brief Access to internal buffer id.
    * TODO Should/can this be hidden?
    */
    GLuint getId() const;

	unsigned int getSize() const;

   private:
    GLuint m_bufferId;
	unsigned int m_size;
    bool m_valid;
};