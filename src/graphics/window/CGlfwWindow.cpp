#include "CGlfwWindow.h"

#include <cassert>

#include "graphics/renderer/core/RendererCoreConfig.h"
#include <GLFW/glfw3.h>

#include "debug/Log.h"

std::unordered_map<GLFWwindow*, CGlfwWindow*> CGlfwWindow::s_windows; /**< Window mapping. */

CGlfwWindow::CGlfwWindow() : m_window(nullptr), m_width(0), m_height(0), m_mouseCaptured(false)
{
    return;
}

CGlfwWindow::~CGlfwWindow()
{
    // Remove mapping
    s_windows.erase(m_window);
}

bool CGlfwWindow::init(unsigned int width, unsigned int height, const std::string& name)
{
    if (!glfwInit())
    {
        LOG_ERROR("Failed to initialize GLFW.");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window from parameters
    m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (m_window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window.");
        glfwTerminate();
        return false;
    }

    m_width = width;
    m_height = height;

    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

// Load OpenGL extensions
#ifndef __APPLE__
    if (flextInit(m_window) != GL_TRUE)
    {
        glfwTerminate();
        LOG_ERROR("Failed to initialize flextGL.");
        return false;
    }
#endif

    // Add mapping
    s_windows[m_window] = this;

    // Set window resize callback
    glfwSetWindowSizeCallback(m_window, &CGlfwWindow::resizeCallback);

    return true;
}

void CGlfwWindow::setWidth(unsigned int width) { m_width = width; }

void CGlfwWindow::setHeight(unsigned int height) { m_height = height; }

unsigned int CGlfwWindow::getWidth() const { return m_width; }

unsigned int CGlfwWindow::getHeight() const { return m_height; }

void CGlfwWindow::setActive() const { glfwMakeContextCurrent(m_window); }

bool CGlfwWindow::isOpen() const { return !glfwWindowShouldClose(m_window); }

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

GLFWwindow* CGlfwWindow::getGlfwHandle() const { return m_window; }

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
