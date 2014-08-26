#include "CSceneObject.h"

CSceneObject::CSceneObject() {}

CSceneObject::~CSceneObject() {}

GLuint CSceneObject::getVertexArrayID() { return m_vertex_array_ID; }

GLuint CSceneObject::getVertexBufferID() { return m_vertex_buffer_ID; }

bool CSceneObject::load(const std::vector<float>& data)
{
    glGenVertexArrays(1, &m_vertex_array_ID);
    glBindVertexArray(m_vertex_array_ID);

    glGenBuffers(1, &m_vertex_buffer_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}