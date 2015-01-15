#include "CCameraController.h"

#include "debug/Log.h"

#include <GLFW/glfw3.h>

#include "graphics/camera/IControllableCamera.h"

#include "input/IInputProvider.h"

CCameraController::CCameraController() {}

CCameraController::~CCameraController()
{
    if (m_inputProvider != nullptr)
    {
        m_inputProvider->removeInputListener(this);
    }
}

void CCameraController::setCamera(std::shared_ptr<IControllableCamera> camera)
{
    m_camera = camera;
}

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
        float walkingModifier = 1.0f;
        if (m_inputProvider->isKeyPressed(GLFW_KEY_SPACE)) {
            walkingModifier = 4.0f;
        }
        
        if (m_inputProvider->isKeyPressed(GLFW_KEY_W))
        {
            m_camera->moveForward(dt * 2.f * m_speed * walkingModifier);
        }
        if (m_inputProvider->isKeyPressed(GLFW_KEY_S))
        {
            m_camera->moveForward(dt * -2.f * m_speed * walkingModifier);
        }
        if (m_inputProvider->isKeyPressed(GLFW_KEY_A))
        {
            m_camera->moveRight(dt * 2.f * m_speed * walkingModifier);
        }
        if (m_inputProvider->isKeyPressed(GLFW_KEY_D))
        {
            m_camera->moveRight(dt * -2.f * m_speed * walkingModifier);
        }

        if (m_inputProvider->isKeyPressed(GLFW_KEY_Q))
        {
            m_camera->moveUp(dt * -2.f * m_speed * walkingModifier);
        }
        if (m_inputProvider->isKeyPressed(GLFW_KEY_E))
        {
            m_camera->moveUp(dt * 2.f * m_speed * walkingModifier);
        }

        if (m_inputProvider->isKeyPressed(GLFW_KEY_UP))
        {
            m_camera->pitch(-dt * m_speed);
        }

        if (m_inputProvider->isKeyPressed(GLFW_KEY_DOWN))
        {
            m_camera->pitch(dt * m_speed);
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
}

void CCameraController::handleKeyEvent(EKeyEventType type, int keyCode)
{
    // TODO
}

void CCameraController::handleMouseMovementEvent(int x, int y)
{
    m_camera->pitch((y - m_lastY) * m_speed / 1000);
    m_camera->yaw(-(x - m_lastX) * m_speed / 1000);
    
    m_lastX = x;
    m_lastY = y;
}

void CCameraController::handleMouseButtonEvent(EMouseButtonEventType type, int buttonCode)
{
    // TODO
}

void CCameraController::handleResizeEvent(int width, int height)
{
	m_camera->setProjection(m_camera->getFieldOfView(), (float)width / height, m_camera->getZNear(), m_camera->getZFar());
}
