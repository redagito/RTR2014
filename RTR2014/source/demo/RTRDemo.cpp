#include <chrono>
#include <future>
#include <thread>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include "demo/RTRDemo.h"

#include <string>
#include <vector>

// Debug
#include "debug/DebugInfo.h"

// Graphics
#include "graphics/renderer/core/RendererCoreConfig.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/FirstPersonCamera.h"

// Renderer types
#include "graphics/renderer/ForwardRenderer.h"
#include "graphics/renderer/DeferredRenderer.h"

// Resource system
#include "resource/Resource.h"

#include "graphics/scene/Scene.h"
#include "graphics/window/GlfwWindow.h"
#include "graphics/GraphicsResourceManager.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/IWindow.h"

#include "util/TimeStamp.h"
#include "graphics/DebugInfoDisplay.h"
#include "input/provider/GlfwInputProvider.h"
#include "io/SceneLoader.h"
#include "demo/CameraController.h"
#include "demo/DemoController.h"

// Animation
#include "animation/AnimationWorld.h"

namespace
{
long elapsedMs(const std::chrono::steady_clock::time_point& start)
{
    return static_cast<long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
            .count());
}
} // namespace

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo()
{
    spdlog::info("Shutting down demo.");

    // The demo controller and the camera controller hold raw pointers to objects
    // owned here, so they have to be destroyed before those objects
    m_demoController.reset();
    m_cameraController.reset();

    // Graphics objects must die while the context is still current
    m_debugInfoDisplay.reset();
    m_renderer.reset();
    m_deferredRenderer.reset();
    m_forwardRenderer.reset();
    m_graphicsResourceManager.reset();

    // The window owns the graphics context
    m_window.reset();
    glfwTerminate();

    spdlog::info("Shutdown complete.");
}

int RTRDemo::init(const std::string& configFile)
{
    auto startTime = std::chrono::steady_clock::now();

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

    m_debugInfo = std::make_shared<DebugInfo>();
    spdlog::default_logger()->sinks().push_back(m_debugInfo);

    // Write the log as it happens, so it survives a crash or a forced exit
    spdlog::flush_on(spdlog::level::info);

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

    // Show a black frame right away, so the window is not blank while loading
    drawLoadingFrame();

    // Create resource manager
    m_resourceManager.reset(createResourceManager());
    if (m_resourceManager == nullptr)
    {
        spdlog::error("Failed to initialize resource manager.");
        return 1;
    }

	// Create animation world
	m_animationWorld = std::make_shared<AnimationWorld>();

    // Graphics resource manager, listens to resource manager
    GraphicsResourceManager* manager = new GraphicsResourceManager;
    m_resourceManager->addResourceListener(manager);
    m_graphicsResourceManager.reset(manager);

    m_camera = std::make_shared<FirstPersonCamera>(
        glm::vec3(0.5f, 0.f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), 45.f,
        4.f / 3.f, 0.01f, 1000.f);

    m_cameraController = std::make_shared<CameraController>();
    m_cameraController->setCamera(m_camera);
    m_cameraController->setInputProvider(m_inputProvider.get());

    m_debugInfoDisplay = std::make_shared<DebugInfoDisplay>(m_resourceManager);

    m_cameraController->setWindow(m_window.get());
    m_window->addListener(m_cameraController.get());

    m_sceneFile = m_config.getValue("scene", "file", "data/scene/test/test_1.json");
    m_sceneLoader = std::make_unique<SceneLoader>(*m_resourceManager);

    spdlog::info("Initialized demo in {} ms.", elapsedMs(startTime));
    return 0;
}

void RTRDemo::drawLoadingFrame()
{
    m_window->setActive();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_window->swapBuffer();
    m_window->pollEvents();
}

int RTRDemo::run()
{
    double timeDiff = 0.0;

    auto loadStart = std::chrono::steady_clock::now();

    // Parse the scene file and decode the resources it references on a worker thread
    std::future<bool> prepared = std::async(std::launch::async, [this]() {
        return m_sceneLoader->prepare(m_sceneFile);
    });

    // Load the resources in steps, keeping the window alive and responsive
    bool resourcesPrepared = false;
    bool renderersCreated = false;
    bool sceneLoaded = false;

    while (!sceneLoaded)
    {
        // Present a frame and process window events
        drawLoadingFrame();

        if (!resourcesPrepared)
        {
            if (prepared.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                if (!prepared.get())
                {
                    spdlog::error("Failed to prepare scene file {}.", m_sceneFile.c_str());
                    return 1;
                }
                resourcesPrepared = true;
                spdlog::info("Decoded scene resources in {} ms.", elapsedMs(loadStart));
            }
            else
            {
                // Do not spin while waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            continue;
        }

        // Create the renderers, they load and compile the shaders
        if (!renderersCreated)
        {
            auto rendererStart = std::chrono::steady_clock::now();
            if (!initRenderer())
            {
                spdlog::error("Failed to initialize renderer.");
                return 1;
            }
            renderersCreated = true;
            spdlog::info("Created renderers in {} ms.", elapsedMs(rendererStart));
            continue;
        }

        // Load the scene, this creates the graphics resources
        auto sceneStart = std::chrono::steady_clock::now();
        if (!initScene())
        {
            spdlog::error("Failed to initialize scene.");
            return 1;
        }
        sceneLoaded = true;
        spdlog::info("Loaded scene in {} ms.", elapsedMs(sceneStart));
    }

    m_demoController = std::make_unique<DemoController>(
        m_deferredRenderer.get(), m_forwardRenderer.get(), m_scene.get(), m_camera.get(),
        m_cameraController.get(), m_inputProvider.get(), m_window.get(),
        m_graphicsResourceManager.get(), m_debugInfo.get(), m_debugInfoDisplay.get(),
        m_animationWorld.get());

    spdlog::info("Startup completed in {} ms.", elapsedMs(loadStart));

    m_window->toggleMouseCapture();

    do
    {
        double startTime = m_window->getTime();

        m_demoController->update((float)timeDiff);

        m_window->swapBuffer();

        // Update input
        m_window->pollEvents();

        timeDiff = m_window->getTime() - startTime;

    } while (!m_inputProvider->isKeyPressed(GLFW_KEY_ESCAPE) && m_window->isOpen());

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
    GlfwWindow* window = new GlfwWindow;
    if (!window->init(width, height, title))
    {
        spdlog::error("Failed to initialize GLFW window wrapper.");
        delete window;
        return false;
    }

    // Set pointer
    m_window.reset(window);

    // Input is read from the window handle
    m_inputProvider = std::make_shared<GlfwInputProvider>(window->getGlfwHandle());

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
    m_deferredRenderer.reset(DeferredRenderer::create(m_resourceManager.get()));
    if (m_deferredRenderer == nullptr)
    {
        spdlog::error("Failed to initialize deferred renderer.");
        return false;
    }

    // Initialize forward renderer
    spdlog::info("Initializing forward renderer.");
    m_forwardRenderer.reset(ForwardRenderer::create(m_resourceManager.get()));
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
    m_scene = std::make_shared<Scene>();

    spdlog::info("Loading initial scene from file {}.", m_sceneFile.c_str());
    if (!m_sceneLoader->load(*m_scene, *m_animationWorld))
    {
        spdlog::error("Failed to load scene file {}.", m_sceneFile.c_str());
        m_scene = nullptr;
        return false;
    }
    return true;
}