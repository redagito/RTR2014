#include "demo/ScreenshotMetadata.h"

#include <json/json.h>

#include <glm/glm.hpp>

#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "graphics/IRenderer.h"
#include "graphics/IWindow.h"
#include "graphics/camera/IControllableCamera.h"

namespace
{
Json::Value toJsonArray(const glm::vec3& value)
{
    Json::Value array(Json::arrayValue);
    array.append(value.x);
    array.append(value.y);
    array.append(value.z);
    return array;
}

Json::Value toJsonArray(const glm::mat4& matrix)
{
    Json::Value rows(Json::arrayValue);
    for (int row = 0; row < 4; ++row)
    {
        Json::Value values(Json::arrayValue);
        for (int column = 0; column < 4; ++column)
        {
            values.append(matrix[column][row]);
        }
        rows.append(values);
    }
    return rows;
}

const char* toJsonValueKind(DebugInfo::ValueKind kind)
{
    switch (kind)
    {
    case DebugInfo::ValueKind::Shortcut:
        return "shortcut";
    case DebugInfo::ValueKind::State:
        return "state";
    }
    return "unknown";
}
}  // namespace

std::string buildScreenshotMetadata(const std::string& imageFile, unsigned long long frame,
                                    const IRenderer& renderer, const IWindow& window,
                                    const IControllableCamera& camera,
                                    const CameraController& cameraController,
                                    const DebugInfo& debugInfo)
{
    Json::Value root;
    root["image"] = imageFile;
    root["frame"] = frame;
    root["window"]["width"] = window.getWidth();
    root["window"]["height"] = window.getHeight();

    root["renderer"] = renderer.getName();

    root["camera"]["position"] = toJsonArray(camera.getPosition());
    root["camera"]["yaw_degrees"] = camera.getYawDegrees();
    root["camera"]["pitch_degrees"] = camera.getPitchDegrees();
    root["camera"]["field_of_view"] = camera.getFieldOfView();
    root["camera"]["z_near"] = camera.getZNear();
    root["camera"]["z_far"] = camera.getZFar();
    root["camera"]["view"] = toJsonArray(camera.getView());
    root["camera"]["projection"] = toJsonArray(camera.getProjection());

    root["mouse"]["x"] = cameraController.getLastMouseX();
    root["mouse"]["y"] = cameraController.getLastMouseY();
    root["mouse"]["captured"] = window.isMouseCaptured();

    root["sequence"]["running"] = cameraController.isSequenceRunning();
    root["sequence"]["time"] = cameraController.getSequenceTime();

    Json::Value values(Json::arrayValue);
    for (const DebugInfo::Value& value : debugInfo.getValues())
    {
        Json::Value entry;
        entry["key"] = value.key;
        entry["setting"] = value.setting;
        entry["status"] = value.status;
        entry["kind"] = toJsonValueKind(value.kind);
        values.append(entry);
    }
    root["debug"] = values;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    return Json::writeString(builder, root);
}
