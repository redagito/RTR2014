#pragma once

#include <vector>

#ifndef __APPLE__
#include "generated/flextGL.h"
#else
#define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

class CSceneObject
{
   public:
    CSceneObject();
    ~CSceneObject();

    bool load(const std::vector<float>& data);
    GLuint getVertexArrayID();
    GLuint getVertexBufferID();

   private:
    GLuint m_vertex_array_ID;
    GLuint m_vertex_buffer_ID;
};
