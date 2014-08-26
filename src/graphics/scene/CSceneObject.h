#pragma once

#include <vector>
#include <memory>

#include "graphics/renderer/core/Config.h"

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
