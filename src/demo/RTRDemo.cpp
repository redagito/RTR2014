#include "RTRDemo.h"

#include <string>
#include <vector>

#include "shaders/Shader.h"
#include "shaders/generated/SimpleShader.h"

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo() {}

int RTRDemo::run()
{
    if (!glfwInit())
    {
        // TODO log
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1024, 768, "RTR Demo", NULL, NULL);
    if (m_window == nullptr)
    {
        // TODO log
        glfwTerminate();
        return -1;
    }
  
    glfwMakeContextCurrent(m_window);

#ifndef __APPLE__
    if (flextInit(m_window) != GL_TRUE) {
        glfwTerminate();
        return 1;
    }
#endif

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.3f, 0.0f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    Shader<SimpleShader> simple;
    simple.init();

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);

    do
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(simple.getProgramID());

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(m_window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_window) == 0);

    glfwTerminate();

    return 0;
}