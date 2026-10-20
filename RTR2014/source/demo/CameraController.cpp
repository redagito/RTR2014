#include "demo/CameraController.h"

#include <GLFW/glfw3.h>
#include <json/json.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <fstream>

#include "graphics/camera/IControllableCamera.h"
#include "input/IInputProvider.h"
#include "io/JsonDeserializer.h"

CameraController::CameraController() {}

CameraController::~CameraController()
{
    if (m_inputProvider != nullptr)
    {
        m_inputProvider->removeInputListener(this);
    }
    if (m_window != nullptr)
    {
        m_window->removeListener(this);
    }
}

void CameraController::setWindow(IWindow* window) { m_window = window; }

void CameraController::setCamera(std::shared_ptr<IControllableCamera> camera) { m_camera = camera; }

void CameraController::setInputProvider(IInputProvider* provider)
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
bool CameraController::loadSequence(std::string file)
{
    m_isRunningSequence = false;
    m_sequenceTime = 0;
    m_sequencePoints.clear();

    Json::Reader reader;
    Json::Value root;

    // Load scene file
    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        spdlog::error("Failed to open json file {}.", file.c_str());
        return false;
    }

    // Parse json data
    if (!reader.parse(ifs, root))
    {
        spdlog::error("Failed to load scene file {} with error {}.", file.c_str(),
                      reader.getFormattedErrorMessages().c_str());
        return false;
    }
    // Read done, close file
    ifs.close();

    Json::Value node = root["positions"];

    // Node empty?
    if (node.empty())
    {
        spdlog::info("Missing or empty 'positions' node. No scene objects are loaded.");
        return true;
    }

    // Node is array type
    if (!node.isArray())
    {
        spdlog::error("The node 'positions' must be array type.");
        return false;
    }

    bool success = true;

    for (unsigned int i = 0; i < node.size(); ++i)
    {
        glm::vec3 position;
        glm::vec3 orientation;
        float timestamp;
        bool fxaaActive;
        bool fogActive;

        if (!loadParam(node[i], "position", position))
        {
            spdlog::error("Failed loading node 'position' for element #{}.", i);
            success = false;
            break;
        }

        if (!loadParam(node[i], "orientation", orientation))
        {
            spdlog::error("Failed loading node 'orientation' for element #{}.", i);
            success = false;
            break;
        }

        if (!loadParam(node[i], "timestamp", timestamp))
        {
            spdlog::error("Failed loading node 'timestamp' for element #{}.", i);
            success = false;
            break;
        }

        if (!loadParam(node[i], "fxaa", fxaaActive))
        {
            spdlog::error("Failed loading node 'fxaa' for element #{}.", i);
            success = false;
            break;
        }

        if (!loadParam(node[i], "fog", fogActive))
        {
            spdlog::error("Failed loading node 'fog' for element #{}.", i);
            success = false;
            break;
        }

        if (glm::dot(orientation, orientation) < 1e-8f)
        {
            spdlog::error("Zero-length orientation for element #{}.", i);
            success = false;
            break;
        }

        // The keyframes are interpolated, so they have to be ordered in time.
        if (!m_sequencePoints.empty() && timestamp <= m_sequencePoints.back().timestamp)
        {
            spdlog::error("The timestamp of element #{} is not later than the one before it.", i);
            success = false;
            break;
        }

        SequencePoint sp{position, glm::normalize(orientation), timestamp, fxaaActive, fogActive};
        m_sequencePoints.push_back(sp);
    }

    m_isRunningSequence = success;
    return success;
}

void CameraController::animate(float dt)
{
    if (m_inputProvider != nullptr && m_camera != nullptr)
    {
        animateFeatures(dt);

        if (m_isRunningSequence)
        {
            animateSequence(dt);
        }
        else
        {
            animateManual(dt);
        }
    }
}

void CameraController::animateFeatures(float dt)
{
    // tODO
}

void CameraController::animateSequence(float dt)
{
    if (m_inputProvider->isKeyPressed(GLFW_KEY_C))
    {
        m_isRunningSequence = false;
        return;
    }

    float timeModifier = 1.0f;
    if (m_inputProvider->isKeyPressed(GLFW_KEY_B))
    {
        timeModifier = -2.0f;
    }
    if (m_inputProvider->isKeyPressed(GLFW_KEY_N))
    {
        timeModifier = 4.0f;
    }
    if (m_inputProvider->isKeyPressed(GLFW_KEY_M))
    {
        timeModifier = 8.0f;
    }

    m_sequenceTime += (dt * timeModifier);
    // Rewinding can push the time below zero; clamp so keyframe lookup works.
    if (m_sequenceTime < 0.0f)
    {
        m_sequenceTime = 0.0f;
    }

    if (m_sequenceTime > m_sequencePoints.back().timestamp)
    {
        // Snap to the final keyframe pose instead of stopping short.
        const SequencePoint& last = m_sequencePoints.back();
        m_camera->lookAt(last.position, last.position + last.orientation);
        m_camera->getFeatureInfoForWrite().fogType =
            (last.fogActive ? FogType::Exp2 : FogType::None);
        m_camera->getFeatureInfoForWrite().fxaaActive = last.fxaaActive;
        m_isRunningSequence = false;
        return;
    }

    auto it = std::find_if(m_sequencePoints.begin(), m_sequencePoints.end(),
                           [&](const SequencePoint& s) { return s.timestamp >= m_sequenceTime; });

    if (it == m_sequencePoints.begin())
    {
        // not started yet
        return;
    }

    SequencePoint after = *it;
    SequencePoint before = *(--it);

    float ix = (m_sequenceTime - before.timestamp) / (after.timestamp - before.timestamp);
    glm::vec3 cameraPosition = before.position * (1.0f - ix) + after.position * ix;
    glm::vec3 cameraOrientation = before.orientation * (1.0f - ix) + after.orientation * ix;

    m_camera->lookAt(cameraPosition, cameraPosition + cameraOrientation);
    m_camera->getFeatureInfoForWrite().fogType = (before.fogActive ? FogType::Exp2 : FogType::None);
    m_camera->getFeatureInfoForWrite().fxaaActive = before.fxaaActive;
}

void CameraController::animateManual(float dt)
{
    float walkingModifier = 1.0f;
    if (m_inputProvider->isKeyPressed(GLFW_KEY_SPACE))
    {
        walkingModifier = 6.0f;
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

void CameraController::handleKeyEvent(KeyEventType type, int keyCode)
{
    if (type != KeyEventType::KEY_PRESSED)
    {
        return;
    }

    if (m_camera == nullptr)
    {
        return;
    }

    FeatureInfo& info = m_camera->getFeatureInfoForWrite();

    if (keyCode == GLFW_KEY_R)
    {
        switch (info.renderMode)
        {
        case RenderMode::Final:
            info.renderMode = RenderMode::Color;
            break;
        case RenderMode::Color:
            info.renderMode = RenderMode::Depth;
            break;
        case RenderMode::Depth:
            info.renderMode = RenderMode::Lights;
            break;
        case RenderMode::Lights:
            info.renderMode = RenderMode::Normals;
            break;
        case RenderMode::Normals:
            info.renderMode = RenderMode::Final;
            break;
        default:
            info.renderMode = RenderMode::Final;
            break;
        }
    }

    if (keyCode == GLFW_KEY_T)
    {
        switch (info.fogType)
        {
        case FogType::None:
            info.fogType = FogType::Linear;
            break;
        case FogType::Linear:
            info.fogType = FogType::Exp;
            break;
        case FogType::Exp:
            info.fogType = FogType::Exp2;
            break;
        case FogType::Exp2:
            info.fogType = FogType::None;
            break;
        default:
            info.fogType = FogType::None;
            break;
        }
    }

    if (keyCode == GLFW_KEY_Z)
    {
        info.shadowsActive = !info.shadowsActive;
    }

    if (keyCode == GLFW_KEY_U)
    {
        info.fxaaActive = !info.fxaaActive;
    }

    if (keyCode == GLFW_KEY_I)
    {
        info.dofActive = !info.dofActive;
    }

    if (keyCode == GLFW_KEY_O)
    {
        info.normalMappingActive = !info.normalMappingActive;
    }
}

void CameraController::handleMouseMovementEvent(int x, int y)
{
    if (m_camera == nullptr)
    {
        return;
    }

    // A jump this large is a cursor warp (e.g. entering capture mode), not
    // real motion; re-baseline instead of pitching/yawing by the full jump.
    const int warpThreshold = 200;
    if (std::abs(x - m_lastX) > warpThreshold || std::abs(y - m_lastY) > warpThreshold)
    {
        m_lastX = x;
        m_lastY = y;
        return;
    }

    m_camera->pitch((y - m_lastY) * m_speed / 1000);
    m_camera->yaw(-(x - m_lastX) * m_speed / 1000);

    m_lastX = x;
    m_lastY = y;
}

void CameraController::handleMouseButtonEvent(MouseButtonEventType type, int buttonCode)
{
    // TODO
}

void CameraController::handleResizeEvent(int width, int height)
{
    if (width <= 0 || height <= 0 || m_camera == nullptr)
    {
        return;
    }
    m_camera->setProjection(m_camera->getFieldOfView(), (float)width / height, m_camera->getZNear(),
                            m_camera->getZFar());
}
