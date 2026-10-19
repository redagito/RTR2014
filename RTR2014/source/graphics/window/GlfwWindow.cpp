#include <spdlog/spdlog.h>
#include "graphics/window/GlfwWindow.h"

#include <cassert>
#include <cstdlib>

#include "graphics/renderer/core/RendererCoreConfig.h"
#include <GLFW/glfw3.h>

namespace
{
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                             GLsizei length, const GLchar* message, const void* userParam)
{
    std::string text = (length > 0) ? std::string(message, length) : std::string(message);

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        spdlog::critical("OpenGL [source {} type {} id {}]: {}", source, type, id, text);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        spdlog::error("OpenGL [source {} type {} id {}]: {}", source, type, id, text);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        spdlog::warn("OpenGL [source {} type {} id {}]: {}", source, type, id, text);
        break;
    default:
        spdlog::debug("OpenGL [source {} type {} id {}]: {}", source, type, id, text);
        break;
    }

    if (type == GL_DEBUG_TYPE_ERROR)
    {
        std::abort();
    }
}
} // namespace


std::unordered_map<GLFWwindow*, GlfwWindow*> GlfwWindow::s_windows; /**< Window mapping. */

GlfwWindow::GlfwWindow() : m_window(nullptr), m_width(0), m_height(0), m_mouseCaptured(false)
{
    return;
}

GlfwWindow::~GlfwWindow()
{
    // Remove mapping
    s_windows.erase(m_window);

    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool GlfwWindow::init(unsigned int width, unsigned int height, const std::string& name)
{
    if (!glfwInit())
    {
        spdlog::error("Failed to initialize GLFW.");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window from parameters
    m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (m_window == nullptr)
    {
        spdlog::error("Failed to create GLFW window.");
        glfwTerminate();
        return false;
    }
    
    int framebufferWidth;
    int framebufferHeight;
    
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    
    m_width = framebufferWidth;
    m_height = framebufferHeight;

    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        glfwTerminate();
        spdlog::error("Failed to initialize glad.");
        return false;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    // Add mapping
    s_windows[m_window] = this;

    // Set window resize callback
    glfwSetFramebufferSizeCallback(m_window, &GlfwWindow::resizeCallback);

    return true;
}

void GlfwWindow::setWidth(unsigned int width) { m_width = width; }

void GlfwWindow::setHeight(unsigned int height) { m_height = height; }

unsigned int GlfwWindow::getWidth() const { return m_width; }

unsigned int GlfwWindow::getHeight() const { return m_height; }

void GlfwWindow::setActive() const { glfwMakeContextCurrent(m_window); }

bool GlfwWindow::isOpen() const { return !glfwWindowShouldClose(m_window); }

void GlfwWindow::swapBuffer() { glfwSwapBuffers(m_window); }

void GlfwWindow::toggleMouseCapture()
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

GLFWwindow* GlfwWindow::getGlfwHandle() const { return m_window; }

void GlfwWindow::addListener(IGlfwWindowListener* l) { m_listeners.insert(l); }

void GlfwWindow::removeListener(IGlfwWindowListener* l) { m_listeners.erase(l); }

void GlfwWindow::resizeCallback(GLFWwindow* window, int width, int height)
{
    assert(width > 0);
    assert(height > 0);
    if (s_windows.count(window) != 0)
    {
        GlfwWindow* win = s_windows.at(window);
        win->handleResize(width, height);
    }
}

void GlfwWindow::handleResize(int width, int height)
{
    setWidth(width);
    setHeight(height);

    for (auto& l : m_listeners)
    {
        l->handleResizeEvent(width, height);
    }
}
