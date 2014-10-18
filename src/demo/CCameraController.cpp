#include "CCameraController.h"

#include "glfw/glfw3.h"

#include "graphics/camera/CFreeCamera.h"

#include "input/IInputProvider.h"

CCameraController::CCameraController() {}

CCameraController::~CCameraController()
{
    if (m_inputProvider != nullptr)
    {
        m_inputProvider->removeInputListener(this);
    }
}

void CCameraController::setCamera(std::shared_ptr<CFreeCamera> camera) { m_camera = camera; }

void CCameraController::setInputProvider(IInputProvider* provider)
{
    if (m_inputProvider != nullptr)
    {
        m_inputProvider->removeInputListener(this);
    }

    m_inputProvider = provider;

    if (m_inputProvider != nullptr)
    {
        m_inputProvider->addInputListener(this);
    }
}

void CCameraController::animate(float dt)
{
    if (m_inputProvider != nullptr && m_camera != nullptr)
    {
        if (m_inputProvider->isKeyPressed(GLFW_KEY_LEFT_ALT) ||
            m_inputProvider->isKeyPressed(GLFW_KEY_RIGHT_ALT))
        {
            if (m_inputProvider->isKeyPressed(GLFW_KEY_UP))
            {
                m_camera->pitch(dt * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_DOWN))
            {
                m_camera->pitch(-dt * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_RIGHT))
            {
                m_camera->yaw(-dt * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_LEFT))
            {
                m_camera->yaw(dt * m_speed);
            }
        }
        else
        {
            if (m_inputProvider->isKeyPressed(GLFW_KEY_UP))
            {
                m_camera->moveUp(dt * 2 * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_DOWN))
            {
                m_camera->moveUp(-dt * 2 * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_RIGHT))
            {
                m_camera->moveRight(dt * 2 * m_speed);
            }

            if (m_inputProvider->isKeyPressed(GLFW_KEY_LEFT))
            {
                m_camera->moveRight(-dt * 2 * m_speed);
            }
        }

        if (m_inputProvider->isKeyPressed(GLFW_KEY_SLASH))
        {
            m_speed -= dt * 2;
        }

        if (m_inputProvider->isKeyPressed(GLFW_KEY_RIGHT_BRACKET))
        {
            m_speed += dt * 2;
        }
    }
}

void CCameraController::handleKeyEvent(EKeyEventType type, int keyCode)
{
    // TODO
}

void CCameraController::handleMouseMovementEvent(int x, int y)
{
    // TODO
}

void CCameraController::handleMouseButtonEvent(EMouseButtonEventType type, int buttonCode)
{
    // TODO
}