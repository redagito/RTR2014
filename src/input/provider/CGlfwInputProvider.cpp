#include "CGlfwInputProvider.h"

#include <cassert>

#include <GLFW/glfw3.h>

#include "debug/Log.h"
#include "input/IInputListener.h"

std::map<GLFWwindow*, CGlfwInputProvider> s_instances;

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        EKeyEventType eventType;

        switch (action)
        {
        case GLFW_PRESS:
            eventType = EKeyEventType::KEY_PRESSED;
            break;
        case GLFW_REPEAT:
            eventType = EKeyEventType::KEY_REPEAT;
            break;
        case GLFW_RELEASE:
            eventType = EKeyEventType::KEY_RELEASED;
            break;
        default:
            LOG_WARNING("Ignored unknown action '%ui' for glfwKeyCallback.", action);
            return;
        }

        for (IInputListener* listener : it->second.m_listeners)
        {
            listener->handleKeyEvent(eventType, key);
        }
    }
}

void glfwCursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        for (IInputListener* listener : it->second.m_listeners)
        {
            listener->handleMouseMovementEvent(xpos, ypos);
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
//            listener->handleMouseMovementEvent(xpos, ypos);
//        }
//    }
//}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto it = s_instances.find(window);
    if (it != s_instances.end())
    {
        EMouseButtonEventType eventType;

        switch (action)
        {
        case GLFW_PRESS:
            eventType = EMouseButtonEventType::MOUSE_BUTTON_PRESSED;
            break;
        case GLFW_RELEASE:
            eventType = EMouseButtonEventType::MOUSE_BUTTON_RELEASED;
            break;
        default:
            LOG_WARNING("Ignored unknown action '%ui' for glfwMouseButtonCallback.", action);
            return;
        }

        for (IInputListener* listener : it->second.m_listeners)
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
// class CGlfwInputProvider
// ===

CGlfwInputProvider& CGlfwInputProvider::createInstance(GLFWwindow* window)
{
    assert(window != nullptr);

    if (s_instances.empty())
    {
        glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetCursorPosCallback(window, glfwCursorPositionCallback);
        glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
    }

    if (s_instances.find(window) == s_instances.end())
    {
        s_instances.emplace(std::piecewise_construct, std::forward_as_tuple(window),
                            std::forward_as_tuple(window));
    }

    return s_instances.at(window);
}

CGlfwInputProvider::CGlfwInputProvider(GLFWwindow* window) : m_window(window)
{
    assert(m_window != nullptr);
}

CGlfwInputProvider::~CGlfwInputProvider() {}

void CGlfwInputProvider::addInputListener(IInputListener* listener)
{
    m_listeners.insert(listener);
}

void CGlfwInputProvider::removeInputListener(IInputListener* listener)
{
    m_listeners.erase(m_listeners.find(listener));
}

bool CGlfwInputProvider::isKeyPressed(int keyCode)
{
    if (m_window != nullptr)
    {
        return glfwGetKey(m_window, keyCode) == GLFW_PRESS;
    }
    return false;
}