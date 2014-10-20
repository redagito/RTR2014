#pragma once

#include <unordered_map>
#include <list>

#include "graphics/IWindow.h"
#include "input/IInputProvider.h"

struct GLFWwindow;

/**
 * \brief GLFW based implementation of the window interface.
 */
class CGlfwWindow : public IWindow
{
   public:
    CGlfwWindow();
    ~CGlfwWindow();

    bool init(unsigned int width, unsigned int height, const std::string& name);

    void setWidth(unsigned int width);
    void setHeight(unsigned int height);

    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void setActive() const;

    bool isOpen() const;

    /**
     * \brief Swaps back and front buffers.
     */
    void swapBuffer();

    /**
    * \brief Toggles mouse capture state.
    */
    void toggleMouseCapture();

    GLFWwindow* getGlfwHandle() const;

   private:
    static void resizeCallback(GLFWwindow* window, int width, int height);

    static std::unordered_map<GLFWwindow*, CGlfwWindow*>
        s_windows; /**< GLFW window to window object mapping. */

    GLFWwindow* m_window;  /**< Window resource. */
    unsigned int m_width;  /**< Current window width. */
    unsigned int m_height; /**< Current window height. */

    bool m_mouseCaptured; /**< Current mouse capture state. */
};
