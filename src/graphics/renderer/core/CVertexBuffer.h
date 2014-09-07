#pragma once

#include <vector>

#include "RendererCoreConfig.h"

/*
* \brief Manages an OpenGL VBO in VRAM.
* Stores float triples (x, y, z) and uses static draw modifier.
*/
class CVertexBuffer
{
   public:
    /**
    * \brief Creates buffer object from float data.
    * \param data Float buffer data.
    * \param usage Buffer usage.
    */
    CVertexBuffer(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);
    CVertexBuffer(const CVertexBuffer& rhs) = delete;

    /**
    * \brief Frees all GPU resources.
    */
    ~CVertexBuffer();

    CVertexBuffer& operator=(const CVertexBuffer& rhs) = delete;

    /**
    * \brief Sets the VBO as active object.
    * Binds the VBO to GL_ARRAY_BUFFER.
    */
    void setActive() const;

	/**
	* \brief Apply for rendering.
	*/
	void apply(GLuint index) const;

    /**
    * \brief Returns buffer validity.
    */
    bool isValid() const;

    /**
    * \brief Access to internal buffer id.
    * TODO Should/can this be hidden?
    */
    GLuint getId() const;

	/**
	* \brief Returns number of elements in the buffer.
	*/
	unsigned int getSize() const;

   private:
    GLuint m_bufferId; /**< GL buffer object id. */
    bool m_valid;
	unsigned int m_size;
};
