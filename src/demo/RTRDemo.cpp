#include "RTRDemo.h"

#include <string>
#include <vector>

#include "debug/Log.h"

#include "graphics/renderer/core/RendererCoreConfig.h"
#include "graphics/renderer/debug/RendererDebug.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/CFirstPersonCamera.h"
#include "graphics/camera/CLookAtCamera.h"

// Renderer types
#include "graphics/renderer/CForwardRenderer.h"
#include "graphics/renderer/CDeferredRenderer.h"

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

    if (!initWindow())
    {
        LOG_ERROR("Failed to initialize window.");
        return 1;
    }

    m_resourceManager = std::make_shared<CResourceManager>();

    if (!initRenderer())
    {
        LOG_ERROR("Failed to initialize renderer.");
        return 1;
    }

    if (!initScene())
    {
        LOG_ERROR("Failed to initialize scene.");
        return 1;
    }

    m_camera = std::make_shared<CLookAtCamera>();
    return 0;
}

int RTRDemo::run()
{
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

        if (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_F1) == GLFW_PRESS && f1Cooldown <= 0.f)
        {
            // Reset cooldown
            f1Cooldown = 0.5f;
            // Capure/uncapture mouse
            m_window->toggleMouseCapture();
        }

        timeDiff = glfwGetTime() - startTime;
    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(m_window->getGlfwHandle()) == 0);

    glfwTerminate();

    return 0;
}

bool RTRDemo::initWindow()
{
    // Check if already initialized
    if (m_window != nullptr)
    {
        return true;
    }

    // Read config values
    unsigned int width = m_config.getValue("window", "width", 800);
    unsigned int height = m_config.getValue("window", "height", 600);
    std::string title = m_config.getValue("window", "title", "Demo");
    LOG_INFO("Creating window with width %u, height %u and title %s.", width, height,
             title.c_str());

    // Create window
    CGlfwWindow* window = new CGlfwWindow;
    if (!window->init(width, height, title))
    {
        LOG_ERROR("Failed to initialize GLFW window wrapper.");
        delete window;
        return false;
    }

    // Set pointer
    m_window.reset(window);
    return true;
}

bool RTRDemo::initRenderer()
{
    if (m_renderer != nullptr)
    {
        LOG_INFO("Renderer already initialized and re-initialization is not supported.");
        return true;
    }

    // Create renderer
    std::string rendererType = m_config.getValue("renderer", "type", "forward");
    LOG_INFO("Renderer type set to %s.", rendererType.c_str());

    // Set renderer object
    if (rendererType == "forward")
    {
        m_renderer = std::make_shared<CForwardRenderer>(m_resourceManager);
    }
    else if (rendererType == "deferred")
    {
        m_renderer = std::make_shared<CDeferredRenderer>(m_resourceManager);
    }
    else
    {
        // Should not happen as default renderer is set to forward in config call
        LOG_WARNING("Invalid renderer type %s specified. Fallback to forward renderer.",
                    rendererType.c_str());
        m_renderer = std::make_shared<CForwardRenderer>(m_resourceManager);
    }

    // Initialize renderer resources
    if (!m_renderer->init())
    {
        LOG_ERROR("Failed to initialize renderer.");
        m_renderer = nullptr;
        return false;
    }

    return true;
}

bool RTRDemo::initScene()
{
    m_scene = std::make_shared<CScene>();
    CSceneLoader loader(*m_resourceManager);

    // Get startup scene from config
    std::string sceneFile = m_config.getValue("scene", "file", "data/scene/test_1.json");
    LOG_INFO("Loading initial scene from file %s.", sceneFile.c_str());
    if (!loader.load(sceneFile, *m_scene))
    {
        LOG_ERROR("Failed to load scene file %s.", sceneFile.c_str());
        m_scene = nullptr;
        return false;
    }
    return true;
}
