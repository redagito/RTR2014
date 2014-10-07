#pragma once

#include <memory>

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

    int init();
    int run();

   private:
    GLFWwindow* m_glfw_window;

    std::shared_ptr<IResourceManager> m_resourceManager = nullptr;
    std::shared_ptr<CGlfwWindow> m_window = nullptr;
    std::shared_ptr<IRenderer> m_renderer = nullptr;
    std::shared_ptr<IScene> m_scene = nullptr;
    std::shared_ptr<CLookAtCamera> m_camera = nullptr;
};
