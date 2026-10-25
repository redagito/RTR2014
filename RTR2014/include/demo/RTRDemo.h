#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "io/IniFile.h"

// Graphics
class IWindow;
class IRenderer;
class IScene;

// Resource
class IResourceManager;

// Animation
class IInputProvider;
class IControllableCamera;
class CameraController;
class IGraphicsResourceManager;
class AnimationWorld;

// Debug
class DebugInfo;
class IDebugInfoDisplay;

// Demo
class DemoController;
class SceneEditor;
class SceneLoader;

// Audio
class SoundEngine;

/**
 * \brief Demo application class.
 *
 * Hosts the platform objects (window, input, renderers) and drives the
 * platform independent demo logic.
 */
class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    /**
     * \brief nitialize demo with config file.
     */
    int init(const std::string& configFile);

    /**
     * \brief Run demo.
     */
    int run();

    /**
     * \brief Renders the given number of frames and writes the last one to a file.
     *
     * The window is not shown and the mouse is not captured, the demo does not
     * take over the desktop. The frames are rendered with a fixed time step, so
     * that the captured image does not depend on the speed of the machine.
     */
    void setFrameCapture(const std::string& file, int frames);

   private:
    bool initWindow();
    bool initRenderer();
    bool initScene();

    /**
     * \brief Writes the rendered image to a file.
     */
    bool captureFrame(const std::filesystem::path& file);

    /**
     * \brief Captures a screenshot and writes the debug metadata beside it.
     */
    bool captureScreenshot();

    /**
     * \brief Writes the debug metadata for a screenshot.
     */
    bool writeScreenshotMetadata(const std::filesystem::path& file, const std::string& imageFile);

    /**
     * \brief Presents a black frame and processes window events.
     */
    void drawLoadingFrame();

    IniFile m_config;

    std::shared_ptr<IResourceManager> m_resourceManager =
        nullptr; /**< Resource loader and manager. */
    std::shared_ptr<IGraphicsResourceManager> m_graphicsResourceManager =
        nullptr; /**< Resource manager for graphics resources. */

    std::shared_ptr<IWindow> m_window = nullptr;
    std::shared_ptr<IInputProvider> m_inputProvider = nullptr;

    std::shared_ptr<IRenderer> m_renderer = nullptr;                /**< Active renderer. */
    std::shared_ptr<IRenderer> m_deferredRenderer = nullptr;        /**< Deferred renderer. */
    std::shared_ptr<IRenderer> m_forwardRenderer = nullptr;         /**< Forward renderer. */
    std::shared_ptr<IScene> m_scene = nullptr;                      /**< Active scene. */
    std::shared_ptr<IControllableCamera> m_camera = nullptr;        /**< Active camera. */
    std::shared_ptr<CameraController> m_cameraController = nullptr; /**< Camera controller. */

    std::shared_ptr<DebugInfo> m_debugInfo = nullptr; /**< Debug information storage. */
    std::shared_ptr<IDebugInfoDisplay> m_debugInfoDisplay =
        nullptr;                                          /**< Debug information renderer. */
    std::shared_ptr<SoundEngine> m_soundEngine = nullptr; /**< Scene background music. */

    // Animation
    std::shared_ptr<AnimationWorld> m_animationWorld;

    // Platform independent demo logic
    std::unique_ptr<DemoController> m_demoController;

    // In-game scene editor, only present when the demo is not capturing a frame
    std::unique_ptr<SceneEditor> m_sceneEditor;

    // Scene loading
    std::unique_ptr<SceneLoader> m_sceneLoader;
    std::string m_sceneFile; /**< Scene file to load. */

    std::filesystem::path m_captureFile; /**< File the captured frame is written to. */
    int m_captureFrames = 0;             /**< Number of frames to render before the capture. */
    int m_renderedFrames = 0;            /**< Number of frames rendered in capture mode. */
};