#pragma once

#include "graphics/renderer/core/RendererCoreConfig.h"

/**
* \brief Write-only render buffer target as frame buffer attachement.
*/
class CRenderBuffer
{
   public:
    /**
    * \brief Creates render buffer resource.
    */
    CRenderBuffer();

    /**
    * \brief Deletes render buffer resource.
    */
    ~CRenderBuffer();

    /**
    * \brief Initializes with dimensions and format.
    */
    bool init(unsigned int width, unsigned int height, GLenum format);

    /**
    * \brief Returns internal resource id.
    */
    GLuint getId() const;

    /**
    * \brief Sets the render buffer active.
    */
    void setActive() const;

   private:
    GLuint m_bufferId = 0;     /**< Render buffer id. */
    unsigned int m_width = 0;  /**< Buffer width. */
    unsigned int m_height = 0; /**< Buffer height. */
};