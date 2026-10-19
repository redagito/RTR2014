#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include "demo/RTRDemo.h"

#include <string>
#include <vector>

// Debug
#include "debug/CDebugInfo.h"

// Graphics
#include "graphics/renderer/core/RendererCoreConfig.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/CFirstPersonCamera.h"
#include "graphics/camera/CFreeCamera.h"

// Renderer types
#include "graphics/renderer/CForwardRenderer.h"
#include "graphics/renderer/CDeferredRenderer.h"

// Resource system
#include "resource/Resource.h"

#include "graphics/scene/CScene.h"
#include "graphics/window/CGlfwWindow.h"
#include "graphics/resource/CGraphicsResourceManager.h"

#include "util/TimeStamp.h"
#include "graphics/CDebugInfoDisplay.h"
#include "input/provider/CGlfwInputProvider.h"
#include "io/CSceneLoader.h"
#include "demo/CCameraController.h"

// Animation
#include "animation/CAnimationWorld.h"

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo()
{
    // GL objects must die while the context is still current, so release the
    // renderers and resource managers before GLFW tears the context down.
    m_debugInfoDisplay.reset();
    m_renderer.reset();
    m_deferredRenderer.reset();
    m_forwardRenderer.reset();
    m_graphicsResourceManager.reset();
    m_window.reset();

    glfwTerminate();
}

int RTRDemo::init(const std::string& configFile)
{
    // Init log file
    std::string logFile = "log/" + createTimeStamp() + ".log";
    try
    {
        spdlog::default_logger()->sinks().push_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        spdlog::warn("Failed to create log file at {}: {}", logFile, ex.what());
    }

    m_debugInfo = std::make_shared<CDebugInfo>();
    spdlog::default_logger()->sinks().push_back(m_debugInfo);

    if (!m_config.load(configFile))
    {
        spdlog::warn("Failed to load config file {}, starting with default settings.",
                    configFile.c_str());
    }

    // Create window for rendering
    if (!initWindow())
    {
        spdlog::error("Failed to initialize window.");
        return 1;
    }

	m_inputProvider = std::make_shared<CGlfwInputProvider>(m_window->getGlfwHandle());

    // Create resource manager
    m_resourceManager.reset(createResourceManager());
    if (m_resourceManager == nullptr)
    {
        spdlog::error("Failed to initialize resource manager.");
        return 1;
    }

	// Create animation world
	m_animationWorld = std::make_shared<CAnimationWorld>();

    // Graphics resource manager, listens to resource manager
    CGraphicsResourceManager* manager = new CGraphicsResourceManager;
    m_resourceManager->addResourceListener(manager);
    m_graphicsResourceManager.reset(manager);

    // Create renderer
    if (!initRenderer())
    {
        spdlog::error("Failed to initialize renderer.");
        return 1;
    }

    if (!initScene())
    {
        spdlog::error("Failed to initialize scene.");
        return 1;
    }

    m_camera = std::make_shared<CFirstPersonCamera>(
        glm::vec3(0.5f, 0.f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), 45.f,
        4.f / 3.f, 0.01f, 1000.f);

    m_cameraController = std::make_shared<CCameraController>();
    m_cameraController->setCamera(m_camera);
    m_cameraController->setInputProvider(m_inputProvider.get());

    m_debugInfoDisplay = std::make_shared<CDebugInfoDisplay>(m_resourceManager);

    m_window->addListener(m_cameraController.get());

    return 0;
}

int RTRDemo::run()
{
    double f1Cooldown = 0.0;
    double f2Cooldown = 0.0;
    double f3Cooldown = 0.0;
    double f5Cooldown = 0.0;
    double k1Cooldown = 0.0;
    double timeDiff = 0.0;

    double fpsCoolDown = 1.f;
    unsigned int lastFrameCount = 0;
    unsigned int currentFrameCount = 0;

    bool displayDebugInfo = false;

    m_window->toggleMouseCapture();
    
    do
    {
        double startTime = glfwGetTime();

        // Cooldowns
        f1Cooldown -= timeDiff;
        f2Cooldown -= timeDiff;
        f3Cooldown -= timeDiff;
        f5Cooldown -= timeDiff;
        k1Cooldown -= timeDiff;
        fpsCoolDown -= timeDiff;

        if (fpsCoolDown < 0)
        {
            fpsCoolDown += 1.f;
            lastFrameCount = currentFrameCount;
            currentFrameCount = 0;
        }

        if (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_1) == GLFW_PRESS && k1Cooldown <= 0.f)
        {
            k1Cooldown = 0.3f;
            displayDebugInfo = !displayDebugInfo;
        }

        if (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_F2) == GLFW_PRESS && f2Cooldown <= 0.f)
        {
            f2Cooldown = 0.3f;
            m_renderer = m_deferredRenderer;
        }

        if (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_F3) == GLFW_PRESS && f3Cooldown <= 0.f)
        {
            f3Cooldown = 0.3f;
            m_renderer = m_forwardRenderer;
        }
        
        if (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_F5) == GLFW_PRESS && f5Cooldown <= 0.f)
        {
            f5Cooldown = 0.3f;
            m_cameraController->loadSequence("data/democam.json");
        }

        m_cameraController->animate((float)timeDiff);

        m_renderer->draw(*m_scene.get(), *m_camera.get(), *m_window.get(),
                         *m_graphicsResourceManager.get());

        if (displayDebugInfo)
        {
            m_debugInfo->setValue("TimeDiff", std::to_string(timeDiff));
            m_debugInfo->setValue("Camera x", std::to_string(m_camera->getPosition().x));
            m_debugInfo->setValue("Camera y", std::to_string(m_camera->getPosition().y));
            m_debugInfo->setValue("Camera z", std::to_string(m_camera->getPosition().z));
            m_debugInfo->setValue("FPS", std::to_string(lastFrameCount));

            m_debugInfoDisplay->draw(*m_debugInfo.get());
        }

		// Perform animation update
		m_animationWorld->update((float) timeDiff);

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
        ++currentFrameCount;

    } while (glfwGetKey(m_window->getGlfwHandle(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(m_window->getGlfwHandle()) == 0);

    return 0;
}

bool RTRDemo::initWindow()
{
    // Check if already initialized
    if (m_window != nullptr)
    {
        return true;
    }

    spdlog::info("Initializing application window.");

    // Read config values
    unsigned int width = m_config.getValue("window", "width", 800);
    unsigned int height = m_config.getValue("window", "height", 600);
    std::string title = m_config.getValue("window", "title", "Demo");

    spdlog::info("Window width: {}.", width);
    spdlog::info("Window height: {}.", height);
    spdlog::info("Window title: {}.", title.c_str());

    // Create window
    CGlfwWindow* window = new CGlfwWindow;
    if (!window->init(width, height, title))
    {
        spdlog::error("Failed to initialize GLFW window wrapper.");
        delete window;
        return false;
    }

    // Set pointer
    m_window.reset(window);
    return true;
}

bool RTRDemo::initRenderer()
{
    if (m_renderer != nullptr || m_deferredRenderer != nullptr || m_forwardRenderer != nullptr)
    {
        spdlog::info("Renderer already initialized and re-initialization is not supported.");
        return true;
    }

    // Initialize deferred renderer
    spdlog::info("Initializing deferred renderer.");
    m_deferredRenderer.reset(CDeferredRenderer::create(m_resourceManager.get()));
    if (m_deferredRenderer == nullptr)
    {
        spdlog::error("Failed to initialize deferred renderer.");
        return false;
    }

    // Initialize forward renderer
    spdlog::info("Initializing forward renderer.");
    m_forwardRenderer.reset(CForwardRenderer::create(m_resourceManager.get()));
    if (m_forwardRenderer == nullptr)
    {
        spdlog::error("Failed to initialize forward renderer.");
        return false;
    }

    // Set renderer
    std::string rendererType = m_config.getValue("renderer", "type", "forward");
    spdlog::info("Initial renderer type set to {}.", rendererType.c_str());

    // Set renderer object
    if (rendererType == "forward")
    {
        m_renderer = m_forwardRenderer;
    }
    else if (rendererType == "deferred")
    {
        m_renderer = m_deferredRenderer;
    }
    else
    {
        // Should not happen as default renderer is set to forward in config call
        spdlog::warn("Invalid renderer type {} specified. Fallback to forward renderer.",
                    rendererType.c_str());
        m_renderer = m_forwardRenderer;
    }

    // Initialize renderer resources
    if (m_renderer == nullptr)
    {
        spdlog::error("Failed to initialize renderer.");
        return false;
    }
    return true;
}

bool RTRDemo::initScene()
{
    m_scene = std::make_shared<CScene>();
    CSceneLoader loader(*m_resourceManager);

    // Get startup scene from config
    std::string sceneFile = m_config.getValue("scene", "file", "data/scene/test/test_1.json");
    spdlog::info("Loading initial scene from file {}.", sceneFile.c_str());
    if (!loader.load(sceneFile, *m_scene, *m_animationWorld))
    {
        spdlog::error("Failed to load scene file {}.", sceneFile.c_str());
        m_scene = nullptr;
        return false;
    }
    return true;
}
