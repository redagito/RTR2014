#include <spdlog/spdlog.h>
#include "demo/DemoController.h"

#include <string>

#include <GLFW/glfw3.h>

#include "animation/AnimationWorld.h"
#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/camera/IControllableCamera.h"
#include "graphics/IRenderer.h"
#include "graphics/IScene.h"
#include "graphics/IWindow.h"
#include "input/IInputProvider.h"

DemoController::DemoController(IRenderer* deferredRenderer, IRenderer* forwardRenderer,
                               IScene* scene, IControllableCamera* camera,
                               CameraController* cameraController, IInputProvider* inputProvider,
                               IWindow* window,
                               IGraphicsResourceManager* graphicsResourceManager,
                               DebugInfo* debugInfo, IDebugInfoDisplay* debugInfoDisplay,
                               AnimationWorld* animationWorld)
    : m_renderer(deferredRenderer),
      m_deferredRenderer(deferredRenderer),
      m_forwardRenderer(forwardRenderer),
      m_scene(scene),
      m_camera(camera),
      m_cameraController(cameraController),
      m_inputProvider(inputProvider),
      m_window(window),
      m_graphicsResourceManager(graphicsResourceManager),
      m_debugInfo(debugInfo),
      m_debugInfoDisplay(debugInfoDisplay),
      m_animationWorld(animationWorld)
{
    return;
}

void DemoController::update(float timeDiff)
{
    updateInput(timeDiff);

    m_cameraController->animate(timeDiff);

    m_renderer->draw(*m_scene, *m_camera, *m_window, *m_graphicsResourceManager);

    updateDebugInfo(timeDiff);

    // Perform animation update
    m_animationWorld->update(timeDiff);

    ++m_currentFrameCount;
}

void DemoController::updateInput(float timeDiff)
{
    // Cooldowns
    m_f1Cooldown -= timeDiff;
    m_f2Cooldown -= timeDiff;
    m_f3Cooldown -= timeDiff;
    m_f5Cooldown -= timeDiff;
    m_k1Cooldown -= timeDiff;
    m_fpsCoolDown -= timeDiff;

    if (m_fpsCoolDown < 0)
    {
        m_fpsCoolDown += 1.0;
        m_lastFrameCount = m_currentFrameCount;
        m_currentFrameCount = 0;
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_1) && m_k1Cooldown <= 0.f)
    {
        m_k1Cooldown = 0.3f;
        m_displayDebugInfo = !m_displayDebugInfo;
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F1) && m_f1Cooldown <= 0.f)
    {
        m_f1Cooldown = 0.5f;
        // Capture/uncapture mouse
        m_window->toggleMouseCapture();
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F2) && m_f2Cooldown <= 0.f)
    {
        m_f2Cooldown = 0.3f;
        m_renderer = m_deferredRenderer;
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F3) && m_f3Cooldown <= 0.f)
    {
        m_f3Cooldown = 0.3f;
        m_renderer = m_forwardRenderer;
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F5) && m_f5Cooldown <= 0.f)
    {
        m_f5Cooldown = 0.3f;
        m_cameraController->loadSequence("data/democam.json");
    }
}

void DemoController::updateDebugInfo(float timeDiff)
{
    if (!m_displayDebugInfo)
    {
        return;
    }

    m_debugInfo->setValue("TimeDiff", std::to_string(timeDiff));
    m_debugInfo->setValue("Camera x", std::to_string(m_camera->getPosition().x));
    m_debugInfo->setValue("Camera y", std::to_string(m_camera->getPosition().y));
    m_debugInfo->setValue("Camera z", std::to_string(m_camera->getPosition().z));
    m_debugInfo->setValue("FPS", std::to_string(m_lastFrameCount));

    m_debugInfoDisplay->draw(*m_debugInfo);
}