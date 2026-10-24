#include "demo/DemoController.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "animation/AnimationWorld.h"
#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "graphics/IDebugInfoDisplay.h"
#include "graphics/IRenderer.h"
#include "graphics/IScene.h"
#include "graphics/IWindow.h"
#include "graphics/camera/IControllableCamera.h"
#include "input/IInputProvider.h"

namespace
{
const char* renderModeName(RenderMode mode)
{
    switch (mode)
    {
    case RenderMode::Final:
        return "Final";
    case RenderMode::Color:
        return "Color";
    case RenderMode::Depth:
        return "Depth";
    case RenderMode::Normals:
        return "Normals";
    case RenderMode::Lights:
        return "Lights";
    }
    return "Unknown";
}

const char* fogTypeName(FogType type)
{
    switch (type)
    {
    case FogType::None:
        return "None";
    case FogType::Linear:
        return "Linear";
    case FogType::Exp:
        return "Exp";
    case FogType::Exp2:
        return "Exp2";
    }
    return "Unknown";
}

std::string formatFloat(float value, int precision)
{
    std::ostringstream out;
    // The overlay and the screenshot metadata use a dot as the decimal separator.
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

std::string formatPosition(const glm::vec3& position)
{
    return "(" + formatFloat(position.x, 2) + ", " + formatFloat(position.y, 2) + ", " +
           formatFloat(position.z, 2) + ")";
}
}  // namespace

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

    m_renderer->draw(*m_scene, *m_camera, *m_window, *m_graphicsResourceManager, timeDiff);

    ++m_frameCount;
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
    m_f12Cooldown -= timeDiff;
    m_fpsCoolDown -= timeDiff;

    if (m_fpsCoolDown < 0)
    {
        // The countdown can be more than a second behind after a long frame.
        m_fpsCoolDown = std::fmod(m_fpsCoolDown, 1.0) + 1.0;
        m_lastFrameCount = m_currentFrameCount;
        m_currentFrameCount = 0;
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
        m_displayDebugInfo = !m_displayDebugInfo;
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F3) && m_f3Cooldown <= 0.f)
    {
        m_f3Cooldown = 0.3f;
        IRenderer* otherRenderer =
            m_renderer == m_deferredRenderer ? m_forwardRenderer : m_deferredRenderer;
        if (otherRenderer != nullptr)
        {
            m_renderer = otherRenderer;
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
        startCameraSequence();
    }

    if (m_inputProvider->isKeyPressed(GLFW_KEY_F12) && m_f12Cooldown <= 0.f)
    {
        m_f12Cooldown = 0.5f;
        m_screenshotRequested = true;
    }
}

void DemoController::startCameraSequence()
{
    m_cameraController->loadSequence("data/democam.json");
}

bool DemoController::consumeScreenshotRequest()
{
    const bool requested = m_screenshotRequested;
    m_screenshotRequested = false;
    return requested;
}

void DemoController::updateDebugInfo(float timeDiff)
{
    if (!m_displayDebugInfo && !m_screenshotRequested)
    {
        return;
    }

    const FeatureInfo& features = m_camera->getFeatureInfo();

    const char* rendererName = "Unknown";
    if (m_renderer == m_deferredRenderer)
    {
        rendererName = "Deferred";
    }
    else if (m_renderer == m_forwardRenderer)
    {
        rendererName = "Forward";
    }

    const glm::vec3& position = m_camera->getPosition();

    std::vector<DebugValue> values;
    values.reserve(32);

    values.push_back({"F1", "Mouse capture", m_window->isMouseCaptured() ? "On" : "Off"});
    values.push_back({"F2", "Debug overlay", m_displayDebugInfo ? "On" : "Off"});
    values.push_back({"F3", "Renderer", rendererName});
    values.push_back({"F4", "VSync", m_vSync ? "On" : "Off"});
    values.push_back(
        {"F5", "Camera sequence", m_cameraController->isSequenceRunning() ? "Running" : "Manual"});
    values.push_back({"F12", "Screenshot", ""});
    values.push_back({"C", "Manual camera", ""});
    values.push_back({"B", "Rewind 2x", ""});
    values.push_back({"N", "Forward 4x", ""});
    values.push_back({"M", "Forward 8x", ""});
    values.push_back({"W/S", "Move forward/back", ""});
    values.push_back({"A/D", "Strafe", ""});
    values.push_back({"Q/E", "Down/up", ""});
    values.push_back({"Space", "Speed 6x", ""});
    values.push_back({"Arrows", "Look", ""});
    values.push_back({"R", "Render mode", renderModeName(features.renderMode)});
    values.push_back({"T", "Fog", fogTypeName(features.fogType)});
    values.push_back({"Z", "Shadows", features.shadowsActive ? "On" : "Off"});
    values.push_back({"U", "FXAA", features.fxaaActive ? "On" : "Off"});
    values.push_back({"I", "Depth of field", features.dofActive ? "On" : "Off"});
    values.push_back({"O", "Normal mapping", features.normalMappingActive ? "On" : "Off"});
    values.push_back({"Esc", "Quit", ""});

    values.push_back({"Frame", "Count", std::to_string(m_frameCount)});
    values.push_back({"FPS", "Rate", std::to_string(m_lastFrameCount)});
    values.push_back({"TimeDiff", "Frame time", formatFloat(timeDiff, 4)});
    values.push_back(
        {"Sequence", "Time", formatFloat(m_cameraController->getSequenceTime(), 2) + " s"});
    values.push_back({"Camera", "Position", formatPosition(position)});
    values.push_back({"Mouse", "X", std::to_string(m_cameraController->getMouseX())});
    values.push_back({"Mouse", "Y", std::to_string(m_cameraController->getMouseY())});
    values.push_back({"Yaw", "Camera", formatFloat(m_camera->getYawDegrees(), 2) + " deg"});
    values.push_back({"Pitch", "Camera", formatFloat(m_camera->getPitchDegrees(), 2) + " deg"});

    m_debugInfo->setValues(std::move(values));

    if (m_displayDebugInfo)
    {
        m_debugInfoDisplay->draw(*m_debugInfo, m_window->getWidth(), m_window->getHeight());
    }
}
