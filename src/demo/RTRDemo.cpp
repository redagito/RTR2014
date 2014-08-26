#include "RTRDemo.h"

#include <string>
#include <vector>

#include "graphics/renderer/core/Config.h"
#include <GLFW/glfw3.h>

#include "graphics/scene/CScene.h"
#include "graphics/scene/CSceneObject.h"

#include "shaders/TShader.h"
#include "shaders/generated/SimpleShader.h"

#include "graphics/window/CGlfwWindow.h"
#include "resource/CResourceManager.h"
#include "graphics/renderer/CRenderer.h"
#include "graphics/camera/CFirstPersonCamera.h"

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
    if (flextInit(m_window) != GL_TRUE)
    {
        glfwTerminate();
        return 1;
    }
#endif

    std::shared_ptr<IResourceManager> resourceManager(new CResourceManager);
    std::shared_ptr<IWindow> window(new CGlfwWindow(m_window));
    std::shared_ptr<IRenderer> renderer(new CRenderer(resourceManager));
    std::shared_ptr<IScene> scene(new CScene);
    std::shared_ptr<ICamera> camera(new CFirstPersonCamera);

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.3f, 0.0f, 0.0f);

    // shader
    TShader<SimpleShader> simple;
    simple.init();

    // scene
    CScene oldscene;

    std::shared_ptr<CSceneObject> object = std::make_shared<CSceneObject>();
    object->load({
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    });

    oldscene.addObject(object);

    do
    {
        // Should be
        renderer->draw(*scene.get(), *camera.get(), *window.get());

        // instead of
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(simple.getProgramID());

        for (const std::shared_ptr<CSceneObject>& object : oldscene.getObjects())
        {
            glBindVertexArray(object->getVertexArrayID());

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, object->getVertexBufferID());
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(m_window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_window) == 0);

    glfwTerminate();

    return 0;
}
