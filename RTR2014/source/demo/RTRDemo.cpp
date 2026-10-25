#include "demo/RTRDemo.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

// Debug
#include "debug/DebugInfo.h"

// Graphics
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "graphics/camera/FirstPersonCamera.h"
#include "graphics/renderer/core/RendererCoreConfig.h"

// Renderer types
#include "graphics/renderer/DeferredRenderer.h"
#include "graphics/renderer/ForwardRenderer.h"

// Resource system
#include "demo/CameraController.h"
#include "demo/DemoController.h"
#include "demo/ScreenshotMetadata.h"
#include "graphics/DebugInfoDisplay.h"
#include "graphics/GraphicsResourceManager.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/IWindow.h"
#include "graphics/scene/Scene.h"
#include "graphics/window/GlfwWindow.h"
#include "input/provider/GlfwInputProvider.h"
#include "io/SceneLoader.h"
#include "resource/Resource.h"
#include "util/FrameTimer.h"
#include "util/Profiler.h"
#include "util/TimeStamp.h"

// Audio
#include "audio/SoundEngine.h"

// Animation
#include "animation/AnimationWorld.h"

namespace
{
// The capture renders the frames with a fixed time step, so that the captured
// image does not depend on the speed of the machine.
constexpr float kCaptureFrameDelta = 1.0f / 60.0f;

// stb writes through a C file stream, which cannot open non-ASCII paths on
// Windows. Writing the encoded PNG to an std::ofstream avoids that limit.
void writeToStream(void* context, void* data, int size)
{
    static_cast<std::ofstream*>(context)->write(static_cast<const char*>(data), size);
}
}  // namespace

RTRDemo::RTRDemo() {}

RTRDemo::~RTRDemo()
{
    spdlog::info("Shutting down demo.");

    // The demo controller and the camera controller hold raw pointers to objects
    // owned here, so they have to be destroyed before those objects
    m_demoController.reset();
    m_cameraController.reset();
    m_soundEngine.reset();

    // Graphics objects must die while the context is still current
    m_debugInfoDisplay.reset();
    m_renderer.reset();
    m_deferredRenderer.reset();
    m_forwardRenderer.reset();
    m_graphicsResourceManager.reset();

    // The window owns the graphics context and the glfw library
    m_window.reset();

    spdlog::info("Shutdown complete.");
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
    {
        ScopedTimer timer(profiler(), "startup: create window");
        if (!initWindow())
        {
            spdlog::error("Failed to initialize window.");
            return 1;
        }
    }

    // Show a black frame right away, so the window is not blank while loading
    drawLoadingFrame();

    // Create resource manager
    m_resourceManager = createResourceManager();

    // Create animation world
    m_animationWorld = std::make_shared<AnimationWorld>();

    // Graphics resource manager, listens to resource manager
    GraphicsResourceManager* manager = new GraphicsResourceManager;
    m_resourceManager->addResourceListener(manager);
    m_graphicsResourceManager.reset(manager);

    // Sound engine; runs silent when no audio device is available. The capture
    // mode stays silent, it must not play sounds on the desktop.
    m_soundEngine = std::make_shared<SoundEngine>();
    if (m_captureFile.empty())
    {
        m_soundEngine->init();
    }

    m_camera = std::make_shared<FirstPersonCamera>(
        glm::vec3(0.5f, 0.f, 0.5f), glm::vec3(0.f, 0.f, 1.f), 45.f, 4.f / 3.f, 0.01f, 1000.f);

    m_cameraController = std::make_shared<CameraController>();
    m_cameraController->setCamera(m_camera);
    m_cameraController->setInputProvider(m_inputProvider.get());

    m_debugInfoDisplay = std::make_shared<DebugInfoDisplay>(m_resourceManager);

    m_cameraController->setWindow(m_window.get());
    m_window->addListener(m_cameraController.get());

    m_sceneFile = m_config.getValue("scene", "file", "data/scene/test/test_1.json");
    m_sceneLoader = std::make_unique<SceneLoader>(*m_resourceManager);

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
    // Parse the scene file and decode the resources it references on a worker thread
    std::future<bool> prepared =
        std::async(std::launch::async,
                   [this]()
                   {
                       ScopedTimer timer(profiler(), "startup: decode resources");
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

        // The window can be closed while the resources are loading.
        if (!m_window->isOpen())
        {
            spdlog::info("The window was closed while the scene was loading.");
            return 0;
        }

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
            ScopedTimer timer(profiler(), "startup: create renderers");
            if (!initRenderer())
            {
                spdlog::error("Failed to initialize renderer.");
                return 1;
            }
            renderersCreated = true;
            continue;
        }

        // Load the scene, this creates the graphics resources
        {
            ScopedTimer timer(profiler(), "startup: load scene");
            if (!initScene())
            {
                spdlog::error("Failed to initialize scene.");
                return 1;
            }
        }
        sceneLoaded = true;
    }

    m_demoController = std::make_unique<DemoController>(
        m_renderer.get(), m_deferredRenderer.get(), m_forwardRenderer.get(), m_scene.get(),
        m_camera.get(), m_cameraController.get(), m_inputProvider.get(), m_window.get(),
        m_graphicsResourceManager.get(), m_debugInfo.get(), m_debugInfoDisplay.get(),
        m_animationWorld.get());

    // The capture mode does not take over the mouse.
    if (m_captureFile.empty())
    {
        m_window->toggleMouseCapture();
    }
    else
    {
        // The frames are rendered as fast as possible, the capture does not
        // have to wait for the display.
        m_window->setVSync(false);
        // The camera follows the sequence of the demo, the mouse does not move
        // it.
        m_demoController->startCameraSequence();
    }

    FrameTimer frameTimer(m_window->getTime());

    do
    {
        // The delta is measured between the frames, not around the work of a
        // single frame, so that it does not depend on how long the frame took.
        // The capture uses a fixed delta, so that the rendered frames do not
        // depend on the speed of the machine.
        const float delta =
            m_captureFile.empty() ? frameTimer.tick(m_window->getTime()) : kCaptureFrameDelta;

        m_demoController->update(delta);

        const glm::mat4& view = m_camera->getView();
        m_soundEngine->updateListener(m_camera->getPosition(),
                                      -glm::vec3(view[0][2], view[1][2], view[2][2]));

        if (m_demoController->consumeScreenshotRequest() && !captureScreenshot())
        {
            spdlog::error("Failed to save the screenshot.");
        }

        // The capture is taken from the back buffer, before it is presented.
        if (!m_captureFile.empty())
        {
            ++m_renderedFrames;
            if (m_renderedFrames >= m_captureFrames)
            {
                if (!captureFrame(m_captureFile))
                {
                    return 1;
                }
                return 0;
            }
        }

        m_window->swapBuffer();

        // Update input
        m_window->pollEvents();

    } while (!m_inputProvider->isKeyPressed(GLFW_KEY_ESCAPE) && m_window->isOpen());

    return 0;
}

void RTRDemo::setFrameCapture(const std::string& file, int frames)
{
    m_captureFile = file;
    m_captureFrames = frames > 0 ? frames : 1;
}

bool RTRDemo::captureFrame(const std::filesystem::path& file)
{
    const unsigned int width = m_window->getWidth();
    const unsigned int height = m_window->getHeight();
    std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 3);
    // The image is in the back buffer of the default framebuffer, it is read
    // before the buffers are swapped.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_RGB,
                 GL_UNSIGNED_BYTE, pixels.data());
    // The first row of the framebuffer is the bottom of the image.
    stbi_flip_vertically_on_write(1);

    std::ofstream out(file, std::ios::binary);
    if (!out.is_open())
    {
        spdlog::error("Failed to open the captured image file {}.", file.string());
        return false;
    }
    const int written = stbi_write_png_to_func(&writeToStream, &out, static_cast<int>(width),
                                               static_cast<int>(height), 3, pixels.data(),
                                               static_cast<int>(width * 3));
    out.close();
    if (written == 0 || !out)
    {
        spdlog::error("Failed to write the captured image to {}.", file.string());
        return false;
    }
    spdlog::info("Wrote the captured image to {}.", file.string());
    return true;
}

bool RTRDemo::captureScreenshot()
{
    const std::filesystem::path directory("screenshots");
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error)
    {
        spdlog::error("Failed to create the screenshot directory {}: {}.", directory.string(),
                      error.message());
        return false;
    }

    const std::string baseName =
        createTimeStamp() + "_" + std::to_string(m_demoController->getFrameCount());
    const std::filesystem::path imageFile = directory / (baseName + ".png");
    const std::filesystem::path metadataFile = directory / (baseName + ".json");

    if (!captureFrame(imageFile))
    {
        return false;
    }

    return writeScreenshotMetadata(metadataFile, imageFile.filename().string());
}

bool RTRDemo::writeScreenshotMetadata(const std::filesystem::path& file,
                                      const std::string& imageFile)
{
    const std::string metadata =
        buildScreenshotMetadata(imageFile, m_demoController->getFrameCount(), *m_renderer,
                                *m_window, *m_camera, *m_cameraController, *m_debugInfo);

    std::ofstream out(file);
    if (!out.is_open())
    {
        spdlog::error("Failed to write the screenshot metadata to {}.", file.string());
        return false;
    }
    out << metadata;
    out.close();
    if (!out)
    {
        spdlog::error("Failed to write the screenshot metadata to {}.", file.string());
        return false;
    }
    spdlog::info("Wrote the screenshot metadata to {}.", file.string());
    return true;
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
    int width = m_config.getValue("window", "width", 800);
    int height = m_config.getValue("window", "height", 600);
    std::string title = m_config.getValue("window", "title", "Demo");

    if (width <= 0 || height <= 0)
    {
        spdlog::error("The window size {}x{} is not valid.", width, height);
        return false;
    }

    spdlog::info("Window width: {}.", width);
    spdlog::info("Window height: {}.", height);
    spdlog::info("Window title: {}.", title.c_str());

    // Create window
    GlfwWindow* window = new GlfwWindow;
    // The capture mode renders without showing the window.
    if (!window->init(width, height, title, m_captureFile.empty()))
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
    m_deferredRenderer =
        DeferredRenderer::create(m_resourceManager.get(), m_sceneLoader->getSkyCubemapFile());
    if (m_deferredRenderer == nullptr)
    {
        spdlog::error("Failed to initialize deferred renderer.");
        return false;
    }

    // Initialize forward renderer
    spdlog::info("Initializing forward renderer.");
    m_forwardRenderer =
        ForwardRenderer::create(m_resourceManager.get(), m_sceneLoader->getSkyCubemapFile());
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

    ResourceId music = m_sceneLoader->getMusicResourceId();
    if (music != invalidResource)
    {
        m_soundEngine->playMusic(music, *m_resourceManager);
    }
    for (const SoundEmitter& emitter : m_sceneLoader->getSoundEmitters())
    {
        m_soundEngine->addEmitter(emitter.m_sound, emitter.m_position, emitter.m_radius,
                                  *m_resourceManager);
    }
    return true;
}