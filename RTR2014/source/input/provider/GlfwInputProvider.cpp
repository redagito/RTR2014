#include "input/provider/GlfwInputProvider.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <cassert>

#include "input/IInputListener.h"

std::unordered_map<GLFWwindow*, GlfwInputProvider*> GlfwInputProvider::s_instances;

void GlfwInputProvider::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action,
                                        int mods)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        KeyEventType eventType;

        switch (action)
        {
        case GLFW_PRESS:
            eventType = KeyEventType::KEY_PRESSED;
            break;
        case GLFW_REPEAT:
            eventType = KeyEventType::KEY_REPEAT;
            break;
        case GLFW_RELEASE:
            eventType = KeyEventType::KEY_RELEASED;
            break;
        default:
            spdlog::warn("Ignored unknown action {} for glfwKeyCallback.", action);
            return;
        }

        for (IInputListener* listener : it->second->m_listeners)
        {
            listener->handleKeyEvent(eventType, key);
        }
    }
}

void GlfwInputProvider::glfwCursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        for (IInputListener* listener : it->second->m_listeners)
        {
            listener->handleMouseMovementEvent(static_cast<int>(xpos), static_cast<int>(ypos));
        }
    }
}

// void glfwCursorEnterCallback(GLFWwindow* window, int entered)
//{
//    auto it = s_instances.find(window);
//    if (it != s_instances.end())
//    {
//        for (IInputListener* listener : it->second.m_listeners)
//        {
//            listener->handleMouseMovementEvent(static_cast<int>(xpos), static_cast<int>(ypos));
//        }
//    }
//}

void GlfwInputProvider::glfwMouseButtonCallback(GLFWwindow* window, int button, int action,
                                                int mods)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        MouseButtonEventType eventType;

        switch (action)
        {
        case GLFW_PRESS:
            eventType = MouseButtonEventType::MOUSE_BUTTON_PRESSED;
            break;
        case GLFW_RELEASE:
            eventType = MouseButtonEventType::MOUSE_BUTTON_RELEASED;
            break;
        default:
            spdlog::warn("Ignored unknown action {} for glfwMouseButtonCallback.", action);
            return;
        }

        for (IInputListener* listener : it->second->m_listeners)
        {
            listener->handleMouseButtonEvent(eventType, button);
        }
    }
}

// void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    auto it = s_instances.find(window);
//    if (it != s_instances.end())
//    {
//        for (IInputListener* listener : it->second.m_listeners)
//        {
//            listener->handleScrollEvent(xoffset, yoffset);
//        }
//    }
//}

// ===
// class GlfwInputProvider
// ===

GlfwInputProvider::GlfwInputProvider(GLFWwindow* window) : m_window(window)
{
    if (m_window == nullptr)
    {
        return;
    }

    // Set callbacks
    glfwSetKeyCallback(m_window, GlfwInputProvider::glfwKeyCallback);
    glfwSetCursorPosCallback(m_window, GlfwInputProvider::glfwCursorPositionCallback);
    glfwSetMouseButtonCallback(m_window, GlfwInputProvider::glfwMouseButtonCallback);
    // Create mapping for callbacks
    s_instances[m_window] = this;
}

GlfwInputProvider::~GlfwInputProvider()
{
    if (m_window == nullptr)
    {
        return;
    }

    // Remove the callbacks, they would outlive this provider.
    glfwSetKeyCallback(m_window, nullptr);
    glfwSetCursorPosCallback(m_window, nullptr);
    glfwSetMouseButtonCallback(m_window, nullptr);
    // Remove mapping
    s_instances.erase(m_window);
}

void GlfwInputProvider::addInputListener(IInputListener* listener)
{
    m_listeners.push_back(listener);
}

void GlfwInputProvider::removeInputListener(IInputListener* listener)
{
    m_listeners.remove(listener);
}

bool GlfwInputProvider::isKeyPressed(int keyCode)
{
    if (m_window != nullptr)
    {
        return glfwGetKey(m_window, keyCode) == GLFW_PRESS;
    }
    return false;
}
