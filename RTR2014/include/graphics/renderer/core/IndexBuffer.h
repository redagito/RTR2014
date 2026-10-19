#pragma once

#include <vector>

#include "RendererCoreConfig.h"

/**
* \brief Represents an index buffer.
*/
class IndexBuffer
{
   public:
    IndexBuffer(const std::vector<unsigned int>& indices, GLenum usage = GL_STATIC_DRAW);

    IndexBuffer(const IndexBuffer& rhs) = delete;

    ~IndexBuffer();

    IndexBuffer& operator=(const IndexBuffer& rhs) = delete;

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
