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
#include "demo/SceneEditor.h"
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
                               AnimationWorld* animationWorld, SceneEditor* sceneEditor)
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
      m_animationWorld(animationWorld),
      m_sceneEditor(sceneEditor)
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

    if (m_sceneEditor != nullptr)
    {
        m_sceneEditor->update(timeDiff);
    }

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

unsigned long long DemoController::getFrameCount() const { return m_frameCount; }

void DemoController::updateDebugInfo(float timeDiff)
{
    const bool editorActive = m_sceneEditor != nullptr && m_sceneEditor->isActive();

    // While the editor is open it owns the overlay, so that its control list is
    // always visible and not pushed around by the demo rows.
    if (editorActive)
    {
        std::vector<DebugInfo::Value> editorValues;
        editorValues.reserve(24);
        m_sceneEditor->appendDebugInfo(editorValues);
        m_debugInfo->setValues(std::move(editorValues));
        m_debugInfoDisplay->draw(*m_debugInfo, m_window->getWidth(), m_window->getHeight());
        return;
    }

    if (!m_displayDebugInfo && !m_screenshotRequested)
    {
        return;
    }

    const FeatureInfo& features = m_camera->getFeatureInfo();

    const char* rendererName = m_renderer->getName();

    const glm::vec3& position = m_camera->getPosition();

    std::vector<DebugInfo::Value> values;
    values.reserve(48);

    const auto shortcut = [](std::string key, std::string setting, std::string status = {})
    {
        return DebugInfo::Value{std::move(key), std::move(setting), std::move(status),
                                DebugInfo::ValueKind::Shortcut};
    };
    const auto state = [](std::string key, std::string setting, std::string status)
    {
        return DebugInfo::Value{std::move(key), std::move(setting), std::move(status),
                                DebugInfo::ValueKind::State};
    };

    values.push_back(shortcut("F1", "Mouse capture", m_window->isMouseCaptured() ? "On" : "Off"));
    values.push_back(shortcut("F2", "Debug overlay", m_displayDebugInfo ? "On" : "Off"));
    values.push_back(shortcut("F3", "Renderer", rendererName));
    values.push_back(shortcut("F4", "VSync", m_vSync ? "On" : "Off"));
    values.push_back(shortcut("F5", "Camera sequence",
                              m_cameraController->isSequenceRunning() ? "Running" : "Manual"));
    values.push_back(shortcut("F12", "Screenshot"));
    values.push_back(shortcut("Tab", "Scene editor", "Off"));
    values.push_back(shortcut("C", "Manual camera"));
    values.push_back(shortcut("B", "Rewind 2x"));
    values.push_back(shortcut("N", "Forward 4x"));
    values.push_back(shortcut("M", "Forward 8x"));
    values.push_back(shortcut("W/S", "Move forward/back"));
    values.push_back(shortcut("A/D", "Strafe"));
    values.push_back(shortcut("Q/E", "Down/up"));
    values.push_back(shortcut("Space", "Speed 6x"));
    values.push_back(shortcut("Arrows", "Look"));
    values.push_back(shortcut("R", "Render mode", renderModeName(features.renderMode)));
    values.push_back(shortcut("T", "Fog", fogTypeName(features.fogType)));
    values.push_back(shortcut("Z", "Shadows", features.shadowsActive ? "On" : "Off"));
    values.push_back(shortcut("U", "FXAA", features.fxaaActive ? "On" : "Off"));
    values.push_back(shortcut("I", "Depth of field", features.dofActive ? "On" : "Off"));
    values.push_back(shortcut("O", "Normal mapping", features.normalMappingActive ? "On" : "Off"));
    values.push_back(shortcut("Esc", "Quit"));

    values.push_back(state("Frame", "Count", std::to_string(m_frameCount)));
    values.push_back(state("FPS", "Rate", std::to_string(m_lastFrameCount)));
    values.push_back(state("TimeDiff", "Frame time", formatFloat(timeDiff, 4)));
    values.push_back(
        state("Sequence", "Time", formatFloat(m_cameraController->getSequenceTime(), 2) + " s"));
    values.push_back(state("Camera", "Position", formatPosition(position)));
    values.push_back(state("Mouse", "X", std::to_string(m_cameraController->getLastMouseX())));
    values.push_back(state("Mouse", "Y", std::to_string(m_cameraController->getLastMouseY())));
    values.push_back(state("Yaw", "Camera", formatFloat(m_camera->getYawDegrees(), 2) + " deg"));
    values.push_back(
        state("Pitch", "Camera", formatFloat(m_camera->getPitchDegrees(), 2) + " deg"));

    m_debugInfo->setValues(std::move(values));

    m_debugInfoDisplay->draw(*m_debugInfo, m_window->getWidth(), m_window->getHeight());
}
