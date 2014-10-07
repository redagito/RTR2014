#include "CGlfwWindow.h"

#include <cassert>

#include <GLFW/glfw3.h>

std::unordered_map<GLFWwindow*, CGlfwWindow*> CGlfwWindow::s_windows; /**< Window mapping. */

CGlfwWindow::CGlfwWindow(GLFWwindow* window) : m_window(window), m_width(0), m_height(0)
{
    assert(m_window != nullptr);

    // Add mapping
    s_windows[m_window] = this;

    // Retrieve size
    int width;
    int height;
    glfwGetWindowSize(m_window, &width, &height);
    // Negative or null values not allowed
    // TODO Why does the function use int instead of unsigned int?
    assert(width > 0);
    assert(height > 0);
    m_width = width;
    m_height = height;

    // Set window resize callback
    glfwSetWindowSizeCallback(m_window, &CGlfwWindow::resizeCallback);
}

CGlfwWindow::~CGlfwWindow()
{
    // Remove mapping
    s_windows.erase(m_window);
}

void CGlfwWindow::setWidth(unsigned int width) { m_width = width; }

void CGlfwWindow::setHeight(unsigned int height) { m_height = height; }

unsigned int CGlfwWindow::getWidth() const { return m_width; }

unsigned int CGlfwWindow::getHeight() const { return m_height; }

void CGlfwWindow::setActive() const { glfwMakeContextCurrent(m_window); }

void CGlfwWindow::swapBuffer() { glfwSwapBuffers(m_window); }

void CGlfwWindow::toggleMouseCapture()
{
    // Set capture mode
    if (m_mouseCaptured)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    // Toggle
    m_mouseCaptured = !m_mouseCaptured;
}

void CGlfwWindow::resizeCallback(GLFWwindow* window, int width, int height)
{
    assert(width > 0);
    assert(height > 0);
    if (s_windows.count(window) != 0)
    {
        CGlfwWindow* win = s_windows.at(window);
        win->setWidth(width);
        win->setHeight(height);
    }
}
