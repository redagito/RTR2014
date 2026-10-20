#include "graphics/window/GlfwWindow.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <cstdlib>

#include "graphics/renderer/core/RendererCoreConfig.h"

namespace
{
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar* message, const void* userParam)
{
    std::string text = (length > 0) ? std::string(message, length) : std::string(message);

    // Performance hints are not errors, whatever severity the driver reports
    if (type == GL_DEBUG_TYPE_PERFORMANCE)
    {
        spdlog::info("OpenGL [source {} type {} id {}]: {}", source, type, id, text);
        return;
    }

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

    if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
    {
        std::abort();
    }
}
}  // namespace

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
        // The window owns the library, it is terminated with the last window.
        glfwTerminate();
    }
}

bool GlfwWindow::init(unsigned int width, unsigned int height, const std::string& name)
{
    if (!glfwInit())
    {
        spdlog::error("Failed to initialize GLFW.");
        return false;
    }

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
    glfwSwapInterval(1);

    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        spdlog::error("Failed to initialize glad.");
        // Destroy this window before glfwTerminate wipes all windows, so the
        // destructor does not touch an already-destroyed handle later.
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
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

void GlfwWindow::setVSync(bool enabled) { glfwSwapInterval(enabled ? 1 : 0); }

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

void GlfwWindow::pollEvents() { glfwPollEvents(); }

double GlfwWindow::getTime() const { return glfwGetTime(); }

void GlfwWindow::addListener(IWindowListener* l) { m_listeners.insert(l); }

void GlfwWindow::removeListener(IWindowListener* l) { m_listeners.erase(l); }

void GlfwWindow::resizeCallback(GLFWwindow* window, int width, int height)
{
    // Minimized windows deliver a 0x0 framebuffer size; nothing to resize to.
    if (width <= 0 || height <= 0)
    {
        return;
    }
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
