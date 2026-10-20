#include <spdlog/spdlog.h>
#include "demo/DemoController.h"

#include <cmath>
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

DemoController::DemoController(IRenderer* initialRenderer, IRenderer* deferredRenderer,
                               IRenderer* forwardRenderer, IScene* scene,
                               IControllableCamera* camera, CameraController* cameraController,
                               IInputProvider* inputProvider, IWindow* window,
                               IGraphicsResourceManager* graphicsResourceManager,
                               DebugInfo* debugInfo, IDebugInfoDisplay* debugInfoDisplay,
                               AnimationWorld* animationWorld)
    : m_renderer(initialRenderer != nullptr ? initialRenderer : deferredRenderer),
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
    if (!hasRequiredDependencies())
    {
        return;
    }

    updateInput(timeDiff);

    m_cameraController->animate(timeDiff);

    // The scene is animated before it is drawn, so that the frame shows the
    // new state of the objects.
    m_animationWorld->update(timeDiff);

    m_renderer->draw(*m_scene, *m_camera, *m_window, *m_graphicsResourceManager);

    updateDebugInfo(timeDiff);

    ++m_currentFrameCount;
}

bool DemoController::hasRequiredDependencies() const
{
    if (m_renderer == nullptr || m_scene == nullptr || m_camera == nullptr || m_window == nullptr ||
        m_graphicsResourceManager == nullptr || m_cameraController == nullptr ||
        m_animationWorld == nullptr)
    {
        spdlog::error("The demo controller is missing a dependency and cannot draw a frame.");
        return false;
    }
    return true;
}

void DemoController::updateInput(float timeDiff)
{
    // Cooldowns
    m_f1Cooldown -= timeDiff;
    m_f2Cooldown -= timeDiff;
    m_f3Cooldown -= timeDiff;
    m_f4Cooldown -= timeDiff;
    m_f5Cooldown -= timeDiff;
    m_k1Cooldown -= timeDiff;
    m_fpsCoolDown -= timeDiff;

    if (m_fpsCoolDown < 0)
    {
        // The countdown can be more than a second behind after a long frame.
        m_fpsCoolDown = std::fmod(m_fpsCoolDown, 1.0) + 1.0;
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
        if (m_deferredRenderer != nullptr)
        {
            m_renderer = m_deferredRenderer;
        }
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F3) && m_f3Cooldown <= 0.f)
    {
        m_f3Cooldown = 0.3f;
        if (m_forwardRenderer != nullptr)
        {
            m_renderer = m_forwardRenderer;
        }
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F4) && m_f4Cooldown <= 0.f)
    {
        m_f4Cooldown = 0.3f;
        m_vSync = !m_vSync;
        m_window->setVSync(m_vSync);
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
    m_debugInfo->setValue("VSync", m_vSync ? "on" : "off");

    m_debugInfoDisplay->draw(*m_debugInfo);
}