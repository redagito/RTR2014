#pragma once

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

   private:
    bool initWindow();
    bool initRenderer();
    bool initScene();

    IniFile m_config;

    std::shared_ptr<IResourceManager> m_resourceManager =
        nullptr; /**< Resource loader and manager. */
    std::shared_ptr<IGraphicsResourceManager> m_graphicsResourceManager =
        nullptr; /**< Resource manager for graphics resources. */
    
	std::shared_ptr<IWindow> m_window = nullptr;
    std::shared_ptr<IInputProvider> m_inputProvider = nullptr;

    std::shared_ptr<IRenderer> m_renderer = nullptr;                 /**< Active renderer. */
    std::shared_ptr<IRenderer> m_deferredRenderer = nullptr;         /**< Deferred renderer. */
    std::shared_ptr<IRenderer> m_forwardRenderer = nullptr;          /**< Forward renderer. */
    std::shared_ptr<IScene> m_scene = nullptr;                       /**< Active scene. */
    std::shared_ptr<IControllableCamera> m_camera = nullptr;         /**< Active camera. */
    std::shared_ptr<CameraController> m_cameraController = nullptr; /**< Camera controller. */

    std::shared_ptr<DebugInfo> m_debugInfo = nullptr; /**< Debug information storage. */
    std::shared_ptr<IDebugInfoDisplay> m_debugInfoDisplay =
        nullptr; /**< Debug information renderer. */

	// Animation
    std::shared_ptr<AnimationWorld> m_animationWorld;

    // Platform independent demo logic
    std::unique_ptr<DemoController> m_demoController;
};