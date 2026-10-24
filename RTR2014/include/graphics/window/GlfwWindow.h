#pragma once

#include <list>
#include <set>
#include <string>
#include <unordered_map>

#include "graphics/IWindow.h"
#include "input/IInputProvider.h"

struct GLFWwindow;

/**
 * \brief GLFW based implementation of the window interface.
 */
class GlfwWindow : public IWindow
{
   public:
    GlfwWindow();
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;

    bool init(unsigned int width, unsigned int height, const std::string& name,
              bool visible = true);

    void setWidth(unsigned int width);
    void setHeight(unsigned int height);

    unsigned int getWidth() const override;
    unsigned int getHeight() const override;

    void setActive() const override;

    bool isOpen() const override;

    /**
     * \brief Swaps back and front buffers.
     */
    void swapBuffer() override;

    void setVSync(bool enabled) override;

    /**
     * \brief Toggles mouse capture state.
     */
    void toggleMouseCapture() override;
    bool isMouseCaptured() const override;

    void pollEvents() override;

    double getTime() const override;

    GLFWwindow* getGlfwHandle() const;

    void addListener(IWindowListener* l) override;
    void removeListener(IWindowListener* l) override;

   private:
    static void resizeCallback(GLFWwindow* window, int width, int height);
    void handleResize(int width, int height);

    static std::unordered_map<GLFWwindow*, GlfwWindow*>
        s_windows; /**< GLFW window to window object mapping. */

    GLFWwindow* m_window;  /**< Window resource. */
    unsigned int m_width;  /**< Current window width. */
    unsigned int m_height; /**< Current window height. */

    bool m_mouseCaptured; /**< Current mouse capture state. */

    std::set<IWindowListener*> m_listeners;
};
