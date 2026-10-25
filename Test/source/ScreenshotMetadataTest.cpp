#include <json/json.h>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <string>

#include "debug/DebugInfo.h"
#include "demo/CameraController.h"
#include "demo/ScreenshotMetadata.h"
#include "graphics/IRenderer.h"
#include "graphics/IWindow.h"
#include "graphics/camera/FirstPersonCamera.h"

namespace
{
class FakeWindow : public IWindow
{
   public:
    unsigned int getWidth() const override { return 800; }
    unsigned int getHeight() const override { return 600; }
    void setActive() const override {}
    bool isOpen() const override { return true; }
    void swapBuffer() override {}
    void setVSync(bool enabled) override {}
    void toggleMouseCapture() override { m_mouseCaptured = !m_mouseCaptured; }
    bool isMouseCaptured() const override { return m_mouseCaptured; }
    void pollEvents() override {}
    double getTime() const override { return 0.0; }
    void addListener(IWindowListener* listener) override {}
    void removeListener(IWindowListener* listener) override {}

   private:
    bool m_mouseCaptured = true;
};

class FakeRenderer : public IRenderer
{
   public:
    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager, float deltaTime) override
    {
    }

    const char* getName() const override { return m_name.c_str(); }

    std::string m_name = "Test Renderer";
};

struct MetadataFixture
{
    MetadataFixture()
    {
        const glm::vec3 eye(1.f, 2.f, 3.f);
        camera = std::make_shared<FirstPersonCamera>(eye, eye + glm::vec3(0.f, 0.f, 1.f), 60.f,
                                                     1.33f, 0.1f, 500.f);

        controller.setCamera(camera);
        controller.handleMouseMovementEvent(120, 80);
        // The mouse movement turns the camera, so aim it afterwards: 90 degrees
        // to the right and 20 degrees up.
        camera->lookAt(
            eye, eye + glm::vec3(glm::cos(glm::radians(20.f)), glm::sin(glm::radians(20.f)), 0.f));

        debugInfo.setValues({
            {"W", "Move forward", "active", DebugInfo::ValueKind::Shortcut},
            {"Speed", "10", "m/s", DebugInfo::ValueKind::State},
        });
    }

    std::string build() const
    {
        return buildScreenshotMetadata("shot.png", 7, renderer, window, *camera, controller,
                                       debugInfo);
    }

    FakeWindow window;
    FakeRenderer renderer;
    std::shared_ptr<FirstPersonCamera> camera;
    CameraController controller;
    DebugInfo debugInfo;
};
}  // namespace

TEST_CASE("Screenshot metadata describes the current frame", "[screenshot]")
{
    MetadataFixture fixture;

    const std::string output = fixture.build();

    Json::Value root;
    Json::Reader reader;
    REQUIRE(reader.parse(output, root));

    CHECK(root["image"].asString() == "shot.png");
    CHECK(root["frame"].asUInt64() == 7);

    CHECK(root["window"]["width"].asUInt() == 800);
    CHECK(root["window"]["height"].asUInt() == 600);

    CHECK(root["renderer"].asString() == "Test Renderer");

    const glm::vec3 position = fixture.camera->getPosition();
    CHECK(root["camera"]["position"][0].asFloat() == Catch::Approx(position.x));
    CHECK(root["camera"]["position"][1].asFloat() == Catch::Approx(position.y));
    CHECK(root["camera"]["position"][2].asFloat() == Catch::Approx(position.z));
    CHECK(root["camera"]["yaw_degrees"].asFloat() == Catch::Approx(90.f).margin(0.01f));
    CHECK(root["camera"]["pitch_degrees"].asFloat() == Catch::Approx(20.f).margin(0.01f));
    CHECK(root["camera"]["field_of_view"].asFloat() == Catch::Approx(60.f));
    CHECK(root["camera"]["z_near"].asFloat() == Catch::Approx(0.1f));
    CHECK(root["camera"]["z_far"].asFloat() == Catch::Approx(500.f));

    const Json::Value& view = root["camera"]["view"];
    REQUIRE(view.size() == 4);
    for (Json::ArrayIndex row = 0; row < 4; ++row)
    {
        REQUIRE(view[row].size() == 4);
        for (Json::ArrayIndex column = 0; column < 4; ++column)
        {
            CHECK(view[row][column].asFloat() ==
                  Catch::Approx(fixture.camera->getView()[column][row]));
        }
    }

    const Json::Value& projection = root["camera"]["projection"];
    REQUIRE(projection.size() == 4);
    for (Json::ArrayIndex row = 0; row < 4; ++row)
    {
        REQUIRE(projection[row].size() == 4);
        for (Json::ArrayIndex column = 0; column < 4; ++column)
        {
            CHECK(projection[row][column].asFloat() ==
                  Catch::Approx(fixture.camera->getProjection()[column][row]));
        }
    }

    CHECK(root["mouse"]["x"].asInt() == 120);
    CHECK(root["mouse"]["y"].asInt() == 80);
    CHECK(root["mouse"]["captured"].asBool());

    CHECK_FALSE(root["sequence"]["running"].asBool());
    CHECK(root["sequence"]["time"].asFloat() == Catch::Approx(0.f));

    const Json::Value& debug = root["debug"];
    REQUIRE(debug.size() == 2);

    CHECK(debug[0]["key"].asString() == "W");
    CHECK(debug[0]["setting"].asString() == "Move forward");
    CHECK(debug[0]["status"].asString() == "active");
    CHECK(debug[0]["kind"].asString() == "shortcut");

    CHECK(debug[1]["key"].asString() == "Speed");
    CHECK(debug[1]["setting"].asString() == "10");
    CHECK(debug[1]["status"].asString() == "m/s");
    CHECK(debug[1]["kind"].asString() == "state");
}

TEST_CASE("Screenshot metadata uses two-space indentation without a trailing newline",
          "[screenshot]")
{
    MetadataFixture fixture;

    const std::string output = fixture.build();

    REQUIRE_FALSE(output.empty());
    CHECK(output.find("\n  \"") != std::string::npos);
    CHECK(output.back() != '\n');
}
