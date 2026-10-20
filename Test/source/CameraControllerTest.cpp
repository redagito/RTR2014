#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include "demo/CameraController.h"
#include "graphics/camera/FirstPersonCamera.h"
#include "input/IInputProvider.h"

namespace
{
class TempJson
{
   public:
    explicit TempJson(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempJson() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_camera.json";
};

class NoInputProvider : public IInputProvider
{
   public:
    void addInputListener(IInputListener*) override {}
    void removeInputListener(IInputListener*) override {}
    bool isKeyPressed(int) override { return false; }
};

std::string keyframe(double timestamp, const std::string& position = "[0, 0, 0]")
{
    return "{\"position\": " + position + ", \"orientation\": [0, 0, 1], \"timestamp\": " +
           std::to_string(timestamp) + ", \"fxaa\": true, \"fog\": false}";
}

std::string sequenceWith(const std::string& keyframes)
{
    return "{\"positions\": [" + keyframes + "]}";
}
} // namespace

TEST_CASE("Camera controller rejects a sequence with two keyframes at the same time", "[demo]")
{
    TempJson file(sequenceWith(keyframe(1.0) + "," + keyframe(1.0)));

    CameraController controller;
    controller.setCamera(std::make_shared<FirstPersonCamera>(
        glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f, 1.33f, 0.01f, 1000.f));

    REQUIRE_FALSE(controller.loadSequence(file.path()));
}

TEST_CASE("Camera controller rejects a sequence whose timestamps go backwards", "[demo]")
{
    TempJson file(sequenceWith(keyframe(1.0) + "," + keyframe(0.0, "[1, 0, 0]")));

    CameraController controller;
    controller.setCamera(std::make_shared<FirstPersonCamera>(
        glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f, 1.33f, 0.01f, 1000.f));

    REQUIRE_FALSE(controller.loadSequence(file.path()));
}

TEST_CASE("Camera controller accepts a sequence with increasing timestamps", "[demo]")
{
    TempJson file(sequenceWith(keyframe(0.0) + "," + keyframe(1.0, "[1, 0, 0]")));

    CameraController controller;
    controller.setCamera(std::make_shared<FirstPersonCamera>(
        glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f, 1.33f, 0.01f, 1000.f));

    REQUIRE(controller.loadSequence(file.path()));
}

TEST_CASE("Camera controller keeps the camera finite while a sequence runs", "[demo]")
{
    TempJson file(sequenceWith(keyframe(0.0)));

    auto camera = std::make_shared<FirstPersonCamera>(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f),
                                                      1.f, 1.33f, 0.01f, 1000.f);
    NoInputProvider inputProvider;
    CameraController controller;
    controller.setCamera(camera);
    controller.setInputProvider(&inputProvider);

    REQUIRE(controller.loadSequence(file.path()));

    controller.animate(0.016f);

    const glm::mat4& view = camera->getView();
    for (int column = 0; column < 4; ++column)
    {
        for (int row = 0; row < 4; ++row)
        {
            REQUIRE(std::isfinite(view[column][row]));
        }
    }
}

TEST_CASE("Camera controller ignores key events without a camera", "[demo]")
{
    CameraController controller;

    // Must not crash
    controller.handleKeyEvent(KeyEventType::KEY_PRESSED, GLFW_KEY_R);
    controller.handleMouseMovementEvent(10, 20);
    controller.handleResizeEvent(800, 600);
}
