#pragma once

#include <memory>
#include <string>

#include "io/CIniFile.h"

struct GLFWwindow;

class IResourceManager;
class CGlfwWindow;
class IRenderer;
class CForwardRenderer;
class CDeferredRenderer;
class IScene;
class IControllableCamera;
class CCameraController;
class IGraphicsResourceManager;

class CDebugInfo;
class CDebugInfoDisplay;

class RTRDemo
{
   public:
    RTRDemo();
    ~RTRDemo();

    int init(const std::string& configFile);
    int run();

   private:
    bool initWindow();
    bool initRenderer();
    bool initScene();

    CIniFile m_config;

    std::shared_ptr<IResourceManager> m_resourceManager =
        nullptr; /**< Resource loader and manager. */
    std::shared_ptr<IGraphicsResourceManager> m_graphicsResourceManager =
        nullptr; /**< Resource manager for graphics resources. */
    std::shared_ptr<CGlfwWindow> m_window = nullptr;
    std::shared_ptr<IRenderer> m_renderer = nullptr;                 /**< Active renderer. */
    std::shared_ptr<IRenderer> m_deferredRenderer = nullptr;         /**< Deferred renderer. */
    std::shared_ptr<IRenderer> m_forwardRenderer = nullptr;          /**< Forward renderer. */
    std::shared_ptr<IScene> m_scene = nullptr;                       /**< Active scene. */
    std::shared_ptr<IControllableCamera> m_camera = nullptr;         /**< Active camera. */
    std::shared_ptr<CCameraController> m_cameraController = nullptr; /**< Camera controller. */

    std::shared_ptr<CDebugInfo> m_debugInfo = nullptr; /**< Debug information storage. */
    std::shared_ptr<CDebugInfoDisplay> m_debugInfoDisplay =
        nullptr; /**< Debug information renderer. */
};
