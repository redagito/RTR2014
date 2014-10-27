#pragma once

#include <memory>
#include <string>

#include "io/CIniFile.h"

struct GLFWwindow;

class IResourceManager;
class CGlfwWindow;
class IRenderer;
class IScene;
class CFreeCamera;
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

    std::shared_ptr<IResourceManager> m_resourceManager = nullptr;
	std::shared_ptr<IGraphicsResourceManager> m_graphicsResourceManager = nullptr;
	std::shared_ptr<CGlfwWindow> m_window = nullptr;
    std::shared_ptr<IRenderer> m_renderer = nullptr;
    std::shared_ptr<IScene> m_scene = nullptr;
    std::shared_ptr<CFreeCamera> m_camera = nullptr;
    std::shared_ptr<CCameraController> m_cameraController = nullptr;
    

    std::shared_ptr<CDebugInfo> m_debugInfo = nullptr;
    std::shared_ptr<CDebugInfoDisplay> m_debugInfoDisplay = nullptr;
};
