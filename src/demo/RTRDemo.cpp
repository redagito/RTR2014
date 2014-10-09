#include "RTRDemo.h"

#include <string>
#include <vector>

#include "debug/Log.h"

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "graphics/renderer/debug/RendererDebug.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/CFirstPersonCamera.h"
#include "graphics/camera/CLookAtCamera.h"
#include "graphics/renderer/CForwardRenderer.h"
#include "graphics/resource/CResourceManager.h"
#include "graphics/scene/CScene.h"
#include "graphics/window/CGlfwWindow.h"

#include "io/CSceneLoader.h"

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo() {}

int RTRDemo::init(const std::string& configFile)
{
    if (!m_config.load(configFile))
    {
        LOG_WARNING("Failed to load config file %s, starting with default settings.",
                    configFile.c_str());
    }

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

    m_glfw_window = glfwCreateWindow(800, 600, "RTR Demo", NULL, NULL);
    if (m_glfw_window == nullptr)
    {
        LOG_ERROR("glfwCreateWindow() failed.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(m_glfw_window);

// TODO This is just ugly...
#ifndef __APPLE__
    if (flextInit(m_glfw_window) != GL_TRUE)
    {
        glfwTerminate();
        return 1;
    }
#endif

    m_resourceManager = std::make_shared<CResourceManager>();
    m_window = std::make_shared<CGlfwWindow>(m_glfw_window);
    m_renderer = std::make_shared<CForwardRenderer>(m_resourceManager);
    m_scene = std::make_shared<CScene>();
    m_camera = std::make_shared<CLookAtCamera>();

    glfwSetInputMode(m_glfw_window, GLFW_STICKY_KEYS, GL_TRUE);

    return 0;
}

int RTRDemo::run()
{
    CSceneLoader loader(*m_resourceManager);
    if (!loader.load(m_config.getValue("scene", "file", "data/scene/test_1.json"), *m_scene))
    {
        LOG_ERROR("Failed to load scene file.");
        return 1;
    }

    // Set camera
    m_camera->setProjection(45.f, 4.f / 3.f, 1.f, 1000.f);
    m_camera->setView(glm::vec3(3.f, 5.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    double f1Cooldown = 0.f;
    double timeDiff = 0;

    do
    {
        double startTime = glfwGetTime();

        // Cooldowns
        f1Cooldown -= timeDiff;

        // Draw call
        m_renderer->draw(*m_scene.get(), *m_camera.get(), *m_window.get());

        // Buffer swap
        m_window->swapBuffer();

        // Update input
        glfwPollEvents();

        if (glfwGetKey(m_glfw_window, GLFW_KEY_F1) == GLFW_PRESS && f1Cooldown <= 0.f)
        {
            // Reset cooldown
            f1Cooldown = 0.5f;
            // Capure/uncapture mouse
            m_window->toggleMouseCapture();
        }

        timeDiff = glfwGetTime() - startTime;
    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_glfw_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_glfw_window) == 0);

    glfwTerminate();

    return 0;
}
