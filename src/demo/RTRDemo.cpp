#include "RTRDemo.h"

#include <string>
#include <vector>

#include "debug/Log.h"

#include "graphics/renderer/core/RendererCoreConfig.h"
#include <GLFW/glfw3.h>

#include "graphics/camera/CFirstPersonCamera.h"
#include "graphics/renderer/CRenderer.h"
#include "graphics/resource/CResourceManager.h"
#include "graphics/scene/CScene.h"
#include "graphics/window/CGlfwWindow.h"

#include "shaders/TShader.h"
#include "shaders/generated/SimpleShader.h"

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo() {}

int RTRDemo::init()
{
    if (!glfwInit())
    {
        LOG_ERROR("glfwInit() failed.");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_glfw_window = glfwCreateWindow(1024, 768, "RTR Demo", NULL, NULL);
    if (m_glfw_window == nullptr)
    {
        LOG_ERROR("glfwCreateWindow() failed.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_glfw_window);

#ifndef __APPLE__
    if (flextInit(m_glfw_window) != GL_TRUE)
    {
        glfwTerminate();
        return 1;
    }
#endif

    m_resourceManager = std::make_shared<CResourceManager>();
    m_window = std::make_shared<CGlfwWindow>(m_glfw_window);
    m_renderer = std::make_shared<CRenderer>(m_resourceManager);
    m_scene = std::make_shared<CScene>();
    m_camera = std::make_shared<CFirstPersonCamera>();

    glfwSetInputMode(m_glfw_window, GLFW_STICKY_KEYS, GL_TRUE);

    return 0;
}

int RTRDemo::run()
{
    glClearColor(0.0f, 0.3f, 0.0f, 0.0f);

    // Create mesh
    ResourceId mesh =
        m_resourceManager->createMesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}, {},
                                      {}, {}, EPrimitiveType::Triangle);

    // Create strings with source code
    ResourceId vso = m_resourceManager->createString(SimpleShader::VS);
    ResourceId fso = m_resourceManager->createString(SimpleShader::FS);
    // Create shader
    ResourceId shader = m_resourceManager->createShader(vso, -1, -1, -1, fso);
    // Create material with custom shader
    ResourceId material = m_resourceManager->createMaterial(-1, -1, -1, -1, -1, shader);
    // Create scene object with mesh and material
    SceneObjectId sceneObj =
        m_scene->createObject(mesh, material, glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f));

    do
    {
        // Should be
        m_renderer->draw(*m_scene.get(), *m_camera.get(), *m_window.get());

        glfwSwapBuffers(m_glfw_window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_glfw_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_glfw_window) == 0);

    glfwTerminate();

    return 0;
}
