#pragma once

#include <memory>
#include <string>

#include "io/CIniFile.h"

struct GLFWwindow;

class IResourceManager;
class CGlfwWindow;
class IRenderer;
class IScene;
class CLookAtCamera;

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

    GLFWwindow* m_glfw_window;

    CIniFile m_config;

    std::shared_ptr<IResourceManager> m_resourceManager = nullptr;
    std::shared_ptr<CGlfwWindow> m_window = nullptr;
    std::shared_ptr<IRenderer> m_renderer = nullptr;
    std::shared_ptr<IScene> m_scene = nullptr;
    std::shared_ptr<CLookAtCamera> m_camera = nullptr;
};
